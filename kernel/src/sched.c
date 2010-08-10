#include "../inc/sched.h"
#include "../inc/heap.h"
#include "../inc/utils.h"
#include "../inc/memlayout.h"
#include "../inc/vmm.h"
#include "../inc/x86.h"
#include "../inc/debug.h"
#include "../inc/io.h"
#include "../inc/elf_helpers.h"
#include "../inc/init.h"

// Un EFLAGS con defaults razonables
#define SCHED_COMMON_EFLAGS 0x3202

extern void load_state();
extern void initialize_task(task_t *task);

// TSS del sistema. Su valor de esp0 se actualiza en los cambios de contexto.
tss_t *tss = NULL;

// Lista de tareas. Inicialmente vacia.
task_t *task_list = NULL;

static void link_tasks(task_t *fst, task_t *sec);

static void initialize_task_state(task_state_t *st, void *entry_point,
    void *stack_pointer);
static void switch_context(task_t *old_task, task_t *new_task);

void sched_init() {

    // Creamos la tarea ``init``
    task_t *init = kmalloc(sizeof(task_t));

    init->prog = kmalloc(sizeof(program_t));
    init->prog->name = "init";

    init->pd = kernel_pd;

    init->kernel_stack = KERNEL_STACK_TOP;
    init->kernel_stack_limit = KERNEL_STACK_BOTTOM;

    // El stack de nivel 0 no interesa. Deberia sobreescribirse al cambiar de
    // tarea. Ademas, como estamos en espacio de kernel, no se deberia utilizar
    // el valor del stack de nivel 0 que esta en la TSS.
    init->kernel_stack_pointer = NULL;
    setup_tss(NULL);

    add_task(init);

    sti();

    init_task();

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

void add_task(task_t *task) {
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
 * - ``pd`` es la direccion virtual del directorio de paginas de la tarea;;
 */
task_t *create_task(uint32_t pd[], struct program_t *prog) {

    task_t *task = kmalloc(sizeof(task_t));

    task->prog = prog;

    task->pd = pd;

    // Alojamos memoria para el stack del kernel de la tarea
    task->kernel_stack = new_kernel_page();
    task->kernel_stack_limit = task->kernel_stack + PAGE_SIZE;
    task->kernel_stack_pointer = task->kernel_stack_limit;

    // Escribimos el task_state_t en la pila del kernel
    void *stack_pointer = elf_stack_bottom(prog->file);
    void *entry_point = elf_entry_point(prog->file);
    task->kernel_stack_pointer -= sizeof(task_state_t);
    task_state_t *st = (task_state_t *)task->kernel_stack_pointer;
    initialize_task_state(st, entry_point, stack_pointer);

    // Al tope de la pila va la direccion de la rutina que inicializa la tarea
    task->kernel_stack_pointer -= 4;
    *((void **)task->kernel_stack_pointer) = task;
    task->kernel_stack_pointer -= 4;
    *((void **)task->kernel_stack_pointer) = initialize_task;
    task->kernel_stack_pointer -= 4;
    *((void **)task->kernel_stack_pointer) = NULL;


    return task;
}

static void switch_context(task_t *old_task, task_t *new_task) {

    // Cargamos el PD de la tarea
    load_cr3((uint32_t)get_kphaddr(new_task->pd));

    // Actualizamos la TSS
    setup_tss((uint32_t)new_task->kernel_stack_limit);

    // Guardamos el stack pointer y cargamos el de la nueva tarea
    switch_stack_pointers(&old_task->kernel_stack_pointer,
        &new_task->kernel_stack_pointer);
}

task_t *current_task() {
    return task_list;
}

void switch_tasks() {
    uint32_t eflags = disable_interrupts();

    task_t *old_task = current_task();
    task_list = old_task->next;

    switch_context(old_task, current_task());

    restore_eflags(eflags);
}

