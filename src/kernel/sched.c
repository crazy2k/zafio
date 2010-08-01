#include "../inc/sched.h"
#include "../inc/types.h"
#include "../inc/memlayout.h"
#include "../inc/mmu.h"

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
void create_tss(void *pd, void *stack_bottom, void *entry_point) {
    tss_t tss1;
    tss_t *tss = &tss1;
    // TODO: Hacer el alloc de la estructura
    //tss_t *tss = alloc(sizeof(tss_t));

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
    tss->es = GDT_SEGSEL(0x3, GDT_INDEX_USER_DS);
    tss->cs = GDT_SEGSEL(0x3, GDT_INDEX_USER_CS);
    tss->ss = GDT_SEGSEL(0x3, GDT_INDEX_USER_DS);
    tss->ds = GDT_SEGSEL(0x3, GDT_INDEX_USER_DS);
    tss->fs = GDT_SEGSEL(0x3, GDT_INDEX_USER_DS);
    tss->gs = GDT_SEGSEL(0x3, GDT_INDEX_USER_DS);
    tss->ldt = NULL;

    tss->t = NULL;   // Debug trap flag
    tss->io = NULL;    // I/O map base address
}
