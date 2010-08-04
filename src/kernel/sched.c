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
extern void *tasktest2();

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

void resume_task(task_t *task) {
    // Cargamos el PD de la tarea
    load_cr3((uint32_t)task->pd);

    // Cargamos los registros en la pila
    BOCHS_BREAK;
    load_state(task);
}

void map_pages(uint32_t pd[], void *vaddr_base, void *phaddr_base, int n,
    uint32_t flags) {

    void *vaddr, *phaddr;
    for (vaddr = vaddr_base, phaddr = phaddr_base;
        vaddr < (vaddr_base + n*PAGE_SIZE);
        vaddr += PAGE_SIZE, phaddr += PAGE_SIZE) {

        uint32_t *pt;
        if (!(pd[PDI(vaddr)] & PDE_P))
            pt = new_page_table(pd, vaddr);
        else
            pt = KVIRTADDR(PDE_PT_BASE(pd[PDI(vaddr)]));
        page_table_map(pt, vaddr, phaddr, flags);
    }
}


void initialize_task(int prog, task_t *task) {

    // Clonamos el PD del kernel
    uint32_t *new_pd = clone_pd(kernel_pd);
    task->pd = get_kphaddr(new_pd);

    task_state_t *st = &task->state;
    
    // Registros de proposito general en cero
    st->eax = NULL;
    st->ecx = NULL;
    st->edx = NULL;
    st->ebx = NULL;
    st->ebp = NULL;
    st->esi = NULL;
    st->edi = NULL;

    // Registros de segmento
    st->es = GDT_SEGSEL(0x3, GDT_INDEX_USER_DS);
    st->ds = GDT_SEGSEL(0x3, GDT_INDEX_USER_DS);
    st->cs = GDT_SEGSEL(0x3, GDT_INDEX_USER_CS);
    st->ss = GDT_SEGSEL(0x3, GDT_INDEX_USER_DS);

    // Flags
    st->eflags = SCHED_COMMON_EFLAGS;

    void *task_code = (prog) ? tasktest : tasktest2;

    st->eip = (uint32_t)KPHADDR(task_code);

    void *addr = (void *)ALIGN_TO_PAGE(KPHADDR(task_code), FALSE);
    map_pages(new_pd, addr, addr, 2, PTE_P | PTE_US);
    map_pages(new_pd, addr + 2*PAGE_SIZE, addr + 2*PAGE_SIZE, 2, PTE_P
        | PTE_US | PTE_RW);

    st->esp = (uint32_t)(addr + 4*PAGE_SIZE);
}

task_t *task1;
task_t *task2;

void sched_start(uint32_t *pd) {
    setup_tss(get_kphaddr(pd));

    task1 = kmalloc(sizeof(task_t));
    initialize_task(1, task1);

    task2 = kmalloc(sizeof(task_t));
    initialize_task(2, task2);


    resume_task(task1);

}

void load_state(task_t *task) {
    __asm__ __volatile__("cli\n"
        "movl %0,%%esp\n"
        "\tpopal\n"
        "\tpopl %%es\n"
        "\tpopl %%ds\n"
        "\tiret"
        : : "g" (task) : "memory");
}
