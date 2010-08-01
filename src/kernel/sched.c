#include "../inc/sched.h"
#include "../inc/types.h"
#include "../inc/memlayout.h"
#include "../inc/mmu.h"
#include "../inc/vmm.h"
#include "../inc/io.h"
#include "../inc/heap.h"

extern void *tasktest();

// Un EFLAGS con defaults razonables
#define SCHED_COMMON_EFLAGS 0x3002

/* Crea la TSS para una tarea a ser cargada.
 *
 * - ``pd`` es la direccion fisica del directorio de paginas de la tarea
 * - ``stack_bottom`` es la direccion del stack pointer de la tarea en su
 *   espacio de direcciones virtual
 * - ``entry_point`` es la direccion del punto de entrada de la tarea en su
 *   espacio de direcciones virtual
 */
tss_t *create_tss(int level, void *pd, void *stack_bottom, void *entry_point) {
    tss_t *tss = (tss_t *)kmalloc(sizeof(tss_t));

    // De momento, no usamos el Previous Task Link
    tss->prev = NULL;

    // Stack para nivel 0
    // TODO: El stack tiene que ser uno nuevo por proceso
    //tss->esp0 = KERNEL_STACK + PAGE_SIZE;
    tss->ss0 = GDT_SEGSEL(0x0, GDT_INDEX_KERNEL_DS);

    // No nos interesan los stacks de niveles 1 y 2
    tss->esp1 = NULL;
    tss->ss1 = GDT_SEGSEL(0x1, GDT_INDEX_NULL);
    tss->esp2 = NULL;
    tss->ss2 = GDT_SEGSEL(0x2, GDT_INDEX_NULL);

    tss->cr3 = (uint32_t)pd;
    tss->eip = (uint32_t)entry_point;

    tss->eflags = SCHED_COMMON_EFLAGS;

    // Registros de proposito general
    tss->eax = NULL;   
    tss->ecx = NULL;
    tss->edx = NULL;
    tss->ebx = NULL;
    tss->esp = (uint32_t)stack_bottom;
    tss->ebp = NULL;
    tss->esi = NULL;
    tss->edi = NULL;

    // Registros de segmento
    uint32_t cs_index = (!level) ? GDT_INDEX_KERNEL_CS : GDT_INDEX_USER_CS;
    uint32_t ds_index = (!level) ? GDT_INDEX_KERNEL_DS : GDT_INDEX_USER_DS;

    tss->es = tss->ss = tss->ds = tss->fs = tss->gs =
        GDT_SEGSEL(level,ds_index);
    tss->cs = GDT_SEGSEL(level, cs_index);

    tss->ldt = NULL;

    tss->t = NULL;  // Debug trap flag
    tss->io = NULL; // I/O map base address
    return tss;
}

void jump_to_segsel(uint16_t segsel) {
    __asm__ __volatile__("pushl %0\n\t"
        "pushl $0\n\t"
        "ljmp *(%%esp)\n\t"
        "addl $8, %%esp"
        : : "rm" (segsel));
}


void tasks_test() {

    // Esbozo de lo usado en un test preliminar

    // un stack a los 10 MB
    //void *stack = new_page(kernel_pd, (void *)0x00A00000, NULL) + PAGE_SIZE;
    //void *stack = kmalloc(PAGE_SIZE);
    void *stack = (void *)KVIRTADDR(0x00A00000);
    tss_t *tss = create_tss(0x0, KPHADDR(kernel_pd), stack, tasktest);

    gdt[GDT_INDEX_TSS] = GDT_DESC_BASE((uint32_t)tss) | GDT_DESC_LIMIT(0x84) |
        GDT_DESC_DPL(0x0) | GDT_DESC_TYPE(GDT_F_32BTA) | GDT_DESC_G |
        GDT_DESC_P;

    uint16_t segsel = GDT_SEGSEL(0x0, GDT_INDEX_TSS);
    jump_to_segsel(segsel);
}

typedef struct {
    // Direcciones virtuales en espacio del kernel de cada seccion
    void *text;         
    void *data;
    // Tamanio de cada seccion
    uint32_t text_size;
    uint32_t data_size;
    // Direccion virtual de carga en espacio de usuario
    void *text_load_addr;
    void *data_load_addr;
    // Direccion virtual del punto de entrada en espacio de usuario
    void *entry_point;
    // Nivel de privilegio del programa
    int level;
} program_t;

void create_task(program_t prog) {

    // Clonamos el PD del kernel
    //uint32_t *new_pd = clone_pd(kernel_pd);

    // Mapeamos una pagina en este nuevo PD para el stack
    //void *stack_uvaddr = new_page(new_pd, SCHED_TASK_STACK_TOP,
    //    PTE_RW | PDE_US | PDE_PWT);

    // Aca habria que:
    // - clonar el pd del kernel o uno de molde,
    // - ubicar en ese pd paginas para el stack y para el codigo y datos de la
    //   tarea,
    // - crear una TSS para la tarea con los datos anteriores
}


