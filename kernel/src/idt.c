#include "../inc/debug.h"
#include "../inc/idt.h"
#include "../inc/memlayout.h"
#include "../inc/mmu.h"
#include "../inc/x86.h"
#include "../inc/io.h"
#include "../inc/sched.h"
#include "../inc/utils.h"
#include "../inc/syscalls.h"
#include "../inc/devices.h"


static void default_isr(uint32_t index, uint32_t error_code, task_state_t *st);
static void timer_isr(uint32_t index, uint32_t error_code, task_state_t *st);
static void pf_isr(uint32_t index, uint32_t error_code, task_state_t *st);
static void syscalls_isr(uint32_t index, uint32_t error_code,
    task_state_t *st);

static void remap_PIC(char offset1, char offset2);

// Arreglo de rutinas de atencion de excepciones/interrupciones
isr_t isrs[IDT_LENGTH] = {NULL};


void idt_init() {

    // Configuramos los handlers en la IDT
    set_handlers();

    // Registramos rutinas de atencion
    register_isr(IDT_INDEX_TIMER, timer_isr);
    register_isr(IDT_INDEX_KB, keyboard_isr);
    register_isr(IDT_INDEX_PF, pf_isr);
    register_isr(IDT_INDEX_SYSC, syscalls_isr);

    // Cargamos la IDT
    lidt(idtr);

    remap_PIC(PIC1_OFFSET, PIC2_OFFSET);

    // Desenmascaramos interrupciones en el PIC
    outb(PIC1_DATA, (~PIC_TIMER) & (~PIC_KB));
}

/* Registra una rutina de atencion ``isr`` para la excepcion/interrupcion cuyo
 * indice en la IDT es ``index``.
 *
 * Si el indice pasado es incorrecto, se devuelve IDT_BAD_INDEX. Si la
 * excepcion/interrupcion ya tiene una rutina de atencion registrada, devuelve
 * IDT_BUSY. Si no hay problemas con el registro, devuelve 0.
 */
int register_isr(uint32_t index, isr_t isr) {

    // Chequeamos si el numero de irq es valido
    if ((index < 0) || (index > IDT_LAST_INDEX))
        return IDT_BAD_INDEX;

    // Chequeamos si la interrupcion ya esta siendo atendida
    if (isrs[index] != NULL)
        return IDT_BUSY;

    isrs[index] = isr;

    return 0;
}

/* Registra un handler (``handler``)  para la excepcion/interrupcion cuyo
 * indice en la IDT es ``index``.
 *
 * Si el indice pasado es incorrecto, se devuelve IDT_BAD_INDEX. Si la
 * excepcion/interrupcion ya tiene un handler registrado, devuelve IDT_BUSY. Si
 * no hay problemas con el registro, se devuelve 0.
 *
 * El handler se registra como Interrupt Gate, de manera que el IF se limpia
 * antes del comienzo de la ejecucion del handler y es responsabilidad de la
 * rutina de servicio la activacion de este si asi lo quiere.
 */
int set_handler(uint32_t index, void (*handler)()) {
    // Chequeamos si el numero de irq es valido
    if ((index < 0) || (index > IDT_LAST_INDEX))
        return IDT_BAD_INDEX;

    // Chequeamos si la interrupcion ya esta siendo atendida
    if (idt[index] & IDT_DESC_P)
        return IDT_BUSY;

    uint64_t dpl = (index == IDT_INDEX_SYSC) ? IDT_DESC_DPL(3) :
        IDT_DESC_DPL(0);

    // Escribimos el descriptor en la IDT
    idt[index] = IDT_DESC_SEL(GDT_SEGSEL(0x0, GDT_INDEX_KERNEL_CS)) |
        IDT_DESC_OFFSET(handler) | IDT_DESC_P | IDT_DESC_D | IDT_DESC_INT |
        dpl;

    return 0;
}

void idt_handle(uint32_t index, uint32_t error_code, task_state_t *st) {
    outb(PIC1_COMMAND, OCW2);

    kill_zombies();

    if (isrs[index] == NULL)
        default_isr(index, error_code, st);
    else
        isrs[index](index, error_code, st);
}

static void remap_PIC(char offset1, char offset2) {
    // Inicializamos PIC1

    // Se envia ICW4, hay varios 8259A, interrupt vectors de 8 bytes,
    // interrupciones por flanco e inicializacion (ICW1)
    outb(PIC1_COMMAND, ICW1_INIT + ICW1_ICW4);
    outb(PIC1_DATA, offset1);       // Offset de interrupciones a la CPU (ICW2)
    outb(PIC1_DATA, ICW3_MATTACH2); // El slave ingresa por IRQ2 (ICW3)
    outb(PIC1_DATA, ICW4_8086);     // Modo 8086 (ICW4)

    // Enmascaramos las interrupciones del PIC1
    outb(PIC1_DATA, 0xFF);

    // Inicializamos PIC2

    outb(PIC2_COMMAND, ICW1_INIT + ICW1_ICW4);  // Idem PIC1 (ICW1)
    outb(PIC2_DATA, offset2);       // Offset de interrupciones a la CPU (ICW2)
    outb(PIC2_DATA, ICW3_SATTACH2); // Se ingresa al PIC1 por IRQ2 (ICW3)
    outb(PIC2_DATA, ICW4_8086);     // Modo 8086 (ICW4)
    
    // Enmascaramos las interrupciones del PIC2
    outb(PIC2_DATA, 0xFF);
}

static void default_isr(uint32_t index, uint32_t error_code, task_state_t *st) {
    kputs("Interrupcion sin rutina de atencion: ");
    kputui32(index);
    kputs("\n");
    BOCHS_BREAK;
}


static uint32_t timer_count = 0;
static void timer_isr(uint32_t index, uint32_t error_code,
    task_state_t *st) {

    timer_count++;

    switch_if_needed(timer_count);
}

#define PF_ISR_P       0x1
#define PF_ISR_WR      0x2
#define PF_ISR_US      0x4
#define PF_ISR_RSVD    0x8
#define PF_ISR_ID      0xC
static void pf_isr(uint32_t index, uint32_t error_code, task_state_t *st) {
    kputs("Fallo de pagina:\n");
    if (!(error_code & PF_ISR_P))
        kputs("- La falla fue causada por una pagina no presente.\n");
    else
        kputs("- La falla fue causada por una violacion de proteccion a nivel"
            "de pagina.\n");

    if (!(error_code & PF_ISR_WR))
        kputs("- El acceso que causo el error fue una lectura.\n");
    else
        kputs("- El acceso que causo el error fue una escritura.\n");

    if (!(error_code & PF_ISR_US))
        kputs("- El acceso ocurrio ejecutando en espacio de supervisor.\n");
    else
        kputs("- El acceso ocurrio ejecutando en espacio de usuario.\n");

    if (!(error_code & PF_ISR_RSVD))
        kputs("- La falla no fue causada por una violacion de bit"
            "reservado.\n");
    else
        kputs("- La falla no fue causada por una violacion de bit"
            "reservado.\n");

    if (!(error_code & PF_ISR_ID))
        kputs("- La falla no fue causada por un fetch de instruccion.\n");
    else
        kputs("- La falla fue causada por un fetch de instruccion.\n");

    kpanic("Fallo de pagina!");
}

static void syscalls_isr(uint32_t index, uint32_t error_code, task_state_t *st) {
    if (st->eax == SYSCALLS_NUM_EXIT)
        sys_exit(current_task());
    else if (st->eax == SYSCALLS_NUM_READ) {
        st->eax = sys_read(st->ebx, (char *)st->ecx, st->edx);
    }
    else if (st->eax == SYSCALLS_NUM_WRITE) {
        st->eax = sys_write(st->ebx, (char *)st->ecx, st->edx);
    }
    else if (st->eax == SYSCALLS_NUM_LS) {
        st->eax = sys_ls(st->ebx, (char *)st->ecx, st->edx);
    }
    else if (st->eax == SYSCALLS_NUM_PS) {
        st->eax = sys_ps(st->ebx, (char *)st->ecx, st->edx);
    }
    else if (st->eax == SYSCALLS_NUM_RUN) {
        st->eax = sys_run((char *)st->ebx);
    }
    else if (st->eax == SYSCALLS_NUM_DEVREQ) {
        st->eax = sys_devreq(st->ebx);
    }
    else if (st->eax == SYSCALLS_NUM_DEVREL) {
        st->eax = sys_devrel(st->ebx);
    }
    else if (st->eax == SYSCALLS_NUM_NICE) {
        st->eax = sys_nice(st->ebx, st->ecx);
    }
    else if (st->eax == SYSCALLS_NUM_WAITPID) {
        st->eax = sys_waitpid(st->ebx);
    }

}
