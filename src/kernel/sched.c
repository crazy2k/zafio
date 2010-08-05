#include "../inc/sched.h"
#include "../inc/heap.h"
#include "../inc/utils.h"
#include "../inc/memlayout.h"
#include "../inc/x86.h"
#include "../inc/debug.h"
#include "../inc/io.h"

// Un EFLAGS con defaults razonables
#define SCHED_COMMON_EFLAGS 0x3202

extern void load_state();

// TSS del sistema. Su valor de esp0 se actualiza en los cambios de contexto.
tss_t *tss = NULL;

// Lista de tareas. Inicialmente vacia.
task_t *task_list = NULL;

static void link_tasks(task_t *fst, task_t *sec);

static void add_task(task_t *task);
static void initialize_task_state(task_state_t *st, void *entry_point,
    void *stack_pointer);
static void switch_stack_pointers(void **old_stack_top, void
    *new_stack_top);
static void switch_context(task_t *old_task, task_t *new_task);

void sched_init() {
    // Alojamos un stack para el kernel en la TSS
    void *kernel_stack = new_kernel_page();
    void *kernel_stack_top = kernel_stack + PAGE_SIZE;
    setup_tss((uint32_t)kernel_stack_top);
}

/* Si no existe la TSS, crea una y la configura con los valores para el
 * segmento y offset del stack de nivel 0, carga un descriptor para ella en la
 * GDT y carga el Task Register con un selector que la referencia.
 *
 * Si ya existe, simplemente actualiza el valor del offset del stack de nivel
 * 0.
 *
 * - ``kernel_stack`` es la direccion del stack pointer para el stack de nivel
 *   0 de la tarea en su espacio de direcciones
 */
void setup_tss(uint32_t kernel_stack) {
    if (tss) {
        tss->esp0 = kernel_stack;
        return;
    }

    tss = (tss_t *)kmalloc(sizeof(tss_t));
    memset(tss, 0, sizeof(tss_t));

    // Solo nos interesa el segmento de stack para el kernel
    tss->ss0 = GDT_SEGSEL(0x0, GDT_INDEX_KERNEL_DS);
    tss->esp0 = kernel_stack;

    // Escribimos el descriptor de la TSS en la GDT
    gdt[GDT_INDEX_TSS] = GDT_DESC_BASE((uint32_t)tss) |
        GDT_DESC_LIMIT(sizeof(tss_t)) | GDT_DESC_DPL(0x0) |
        GDT_DESC_TYPE(GDT_F_32BTA) | GDT_DESC_G | GDT_DESC_P;

    // Cargamos el Task Register con un selector para la TSS
    uint16_t segsel = GDT_SEGSEL(0x0, GDT_INDEX_TSS);
    ltr(segsel);
}

static void link_tasks(task_t *fst, task_t *sec) {
    fst->next = sec;
    sec->prev = fst;
}

static void add_task(task_t *task) {
    if (!task_list) {
        task_list = task;
        link_tasks(task, task);
    }
    else {
        link_tasks(task, task_list->next);
        link_tasks(task_list, task);
    }
}

/* Inicializa un estado de tarea con los siguientes valores iniciales:
 * - los registros de proposito general en 0;
 * - los registros de segmento tienen como valor un selector que
 *   referencia los segmentos de usuario en la GDT;
 * - los flags adquieren el valor SCHED_COMMON_EFLAGS;
 * - el instruction pointer contiene el valor del punto de entrada
 *   ``entry_point``;
 * - el stack pointer contiene el valor de ``stack_pointer``.
 *
 * Tener en cuenta que tanto ``entry_point`` como ``stack_pointer`` deben
 * ser direcciones en el espacio de direcciones virtual de la tarea.
 */
static void initialize_task_state(task_state_t *st, void *entry_point,
    void *stack_pointer) {

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

    st->eip = (uint32_t)entry_point;

    st->esp = (uint32_t)stack_pointer;
}

/* Crea una nueva tarea lista para ser ejecutada.
 * - ``pd`` es la direccion fisica del directorio de paginas de la tarea;;
 * - ``entry_point`` es el punto de entrada de la tarea en su espacio de
 *   direcciones virtual;
 * - ``stack_pointer`` es el stack pointer inicial de la tarea en su espacio
 *   de direcciones virtual.
 */
task_t *create_task(uint32_t pd[], int level, void *entry_point,
    void *stack_pointer) {

    task_t *task = kmalloc(sizeof(task_t));

    task->pd = pd;

    // Alojamos memoria para el stack del kernel de la tarea
    task->kernel_stack = new_kernel_page();
    task->kernel_stack_top = task->kernel_stack + PAGE_SIZE;

    // Escribimos el task_state_t en la pila del kernel
    task->kernel_stack_top -= sizeof(task_state_t);
    task_state_t *st = (task_state_t *)task->kernel_stack_top;
    initialize_task_state(st, entry_point, stack_pointer);

    // Al tope de la pila va la direccion de la rutina que carga el estado
    --(task->kernel_stack_top);
    task->kernel_stack_top = load_state;

    return task;
}

static void switch_stack_pointers(void **old_stack_top, void *new_stack_top) {
    __asm__ __volatile__("mov %%esp, %0" : "=m" (old_stack_top): );
    __asm__ __volatile__("mov %0, %%esp" : : "m" (old_stack_top));
}

static void switch_context(task_t *old_task, task_t *new_task) {

    // Cargamos el PD de la tarea
    load_cr3((uint32_t)new_task->pd);

    // Actualizamos la TSS
    setup_tss((uint32_t)new_task->kernel_stack_top);

    // Guardamos el stack pointer y cargamos el de la nueva tarea
    switch_stack_pointers(&old_task->kernel_stack_top,
        new_task->kernel_stack_top);
}

void resched() {
    task_t *old_task = task_list;
    task_list = old_task->next;

    switch_context(old_task, task_list);
}

