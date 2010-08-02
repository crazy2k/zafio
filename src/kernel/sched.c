#include "../inc/sched.h"
#include "../inc/types.h"
#include "../inc/memlayout.h"
#include "../inc/mmu.h"
#include "../inc/vmm.h"
#include "../inc/io.h"
#include "../inc/heap.h"
#include "../inc/x86.h"
#include "../inc/debug.h"
#include "../inc/utils.h"

extern void *tasktest();

// Un EFLAGS con defaults razonables
#define SCHED_COMMON_EFLAGS 0x3202

/* Configura una TSS con valores para el segmento y offset del stack de nivel
 * 0, carga un descriptor para ella en la GDT y carga el Task Register con un
 * selector que la referencia.
 *
 * - ``pd`` es la direccion fisica del directorio de paginas del kernel
 */
void setup_tss(void *pd) {
    tss_t *tss = (tss_t *)kmalloc(sizeof(tss_t));
    memset(tss, 0, sizeof(tss_t));

    // Solo nos interesan los campos del stack del kernel
    tss->ss0 = GDT_SEGSEL(0x0, GDT_INDEX_KERNEL_DS);
    tss->esp0 = (uint32_t)(KERNEL_STACK_TOP + KERNEL_STACK_SIZE);

    // Escribimos el descriptor de la TSS en la GDT
    gdt[GDT_INDEX_TSS] = GDT_DESC_BASE((uint32_t)tss) |
        GDT_DESC_LIMIT(sizeof(tss_t)) | GDT_DESC_DPL(0x0) |
        GDT_DESC_TYPE(GDT_F_32BTA) | GDT_DESC_G | GDT_DESC_P;

    // Cargamos el Task Register con un selector para la TSS
    uint16_t segsel = GDT_SEGSEL(0x0, GDT_INDEX_TSS);
    ltr(segsel);

}

void create_task(void *prog) {

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

void resume_task(task_t *task) {
    // Cargamos el PD de la tarea
    load_cr3((uint32_t)task->pd);

    // Cargamos los registros en la pila
    load_state(task);
}

void new_task_test() {
    BOCHS_BREAK;
    // Clonamos el PD del kernel
    uint32_t *new_pd = clone_pd(kernel_pd);
    task_state_t st;
    st.eax = NULL;
    st.ecx = NULL;
    st.edx = NULL;
    st.ebx = NULL;
    st.ebp = NULL;
    st.esi = NULL;
    st.edi = NULL;
    st.es = GDT_SEGSEL(0x0, GDT_INDEX_KERNEL_DS);
    st.ds = GDT_SEGSEL(0x0, GDT_INDEX_KERNEL_DS);
    st.eip = (uint32_t)tasktest;
    st.cs = GDT_SEGSEL(0x0, GDT_INDEX_KERNEL_CS);
    st.eflags = SCHED_COMMON_EFLAGS;
    st.esp = 0xC0A00000;
    st.ds = GDT_SEGSEL(0x0, GDT_INDEX_KERNEL_DS);

    task_t *task = kmalloc(sizeof(task_t));
    task->pd = KPHADDR(new_pd);
    task->state = st;
    resume_task(task);
}

void *get_phys_addr(void *vaddr) {
    return KPHADDR(vaddr);
}

void sched_start(uint32_t *pd) {
    setup_tss(get_phys_addr(pd));
    new_task_test();
}
