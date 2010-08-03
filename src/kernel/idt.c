#include "../inc/debug.h"
#include "../inc/idt.h"
#include "../inc/memlayout.h"
#include "../inc/mmu.h"
#include "../inc/x86.h"
#include "../inc/io.h"
#include "../inc/sched.h"

void idt_init() {

    register_handler(IDT_INDEX_PF, idt_pf_handler, IDT_DESC_INT);
    register_handler(IDT_INDEX_TIMER, idt_timer_handler, IDT_DESC_INT);

    lidt(idtr);

    remap_PIC(PIC1_OFFSET, PIC2_OFFSET);
    //outb(PIC1_DATA, 0xFC);

    //sti();
}

/* Registra un handler (``handler``)  para la excepcion/interrupcion cuyo
 * indice en la IDT es ``index``. ``type`` puede contener alguno de los valores
 * IDT_DESC_INT, IDT_DESC_TRAP o cero; en ese ultimo caso, se asumira que
 * ``type`` es IDT_DESC_INT.
 *
 * Si el indice pasado es incorrecto, se devuelve IDT_BAD_INDEX. Si la
 * excepcion/interrupcion ya tiene un handler registrado, devuelve IDT_BUSY. Si
 * no hay problemas con el registro, se devuelve 0.
 */
int register_handler(uint32_t index, void (*handler)(), uint64_t type) {
    // Chequeamos si el numero de irq es valido
    if ((index < 0) || (index > IDT_LAST_INDEX))
        return IDT_BAD_INDEX;

    // Chequeamos si la interrupcion ya esta siendo atendida
    if (idt[index] & IDT_DESC_P)
        return IDT_BUSY;

    if (!type)
        type = IDT_DESC_INT;

    // Escribimos el descriptor en la IDT
    idt[index] = IDT_DESC_SEL(GDT_SEGSEL(0x0, GDT_INDEX_KERNEL_CS)) |
        IDT_DESC_OFFSET(handler) | IDT_DESC_P | IDT_DESC_DPL(0) |
        IDT_DESC_D | type;

    return 0;
}

void idt_pf_handler() {
    BOCHS_BREAK;
}

extern task_t *task2;

int i = 0;
void idt_timer_handler() {
    outb(PIC1_COMMAND, OCW2);
    i++;
    BOCHS_BREAK;
    if (i == 999) {

        resume_task(task2);
    }
    __asm__ __volatile__("iret");
}


void remap_PIC(char offset1, char offset2) {
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
