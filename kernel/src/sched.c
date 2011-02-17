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
#include "../inc/devices.h"

// Un EFLAGS con defaults razonables
#define SCHED_COMMON_EFLAGS 0x3202

extern void load_state();
extern void initialize_task(task_t *task);
extern void wait_for_interrupt();
extern void start_task(int (*main)());

// TSS del sistema. Su valor de esp0 se actualiza en los cambios de contexto.
tss_t *tss = NULL;

// Lista de tareas. Inicialmente vacia.
task_t *task_list = NULL;

// Tarea para ser removida. Inicialmente ninguna.
task_t *zombie_task = NULL;

//Contador de process pids
uint32_t pid_counter = 0;

static void link_tasks(task_t *fst, task_t *sec);

static void initialize_task_state(task_state_t *st, void *entry_point,
    void *stack_pointer);
static void switch_context(task_t *old_task, task_t *new_task);
static void restart_quantum(task_t *task);
static uint32_t get_next_pid();

void sched_init() {

    // Creamos la tarea ``init``
    task_t *init = kmalloc(sizeof(task_t));

    init->prog = kmalloc(sizeof(program_t));
    init->prog->name = "init";

    init->pd = kernel_pd;

    init->kernel_stack = KERNEL_STACK_TOP;
    init->kernel_stack_limit = KERNEL_STACK_BOTTOM;

    init->waiting = FALSE;
    init->waited = FALSE;
    init->parent = NULL;

    init->ticks = 0;
    init->quantum = SCHED_QUANTUM;
    restart_quantum(init);
    
    init->pid = get_next_pid();

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

void remove_task(task_t *task) {
    task->next->prev = task->prev;
    task->prev->next = task->next;
}

void put_zombie(task_t *task) {
    zombie_task = task;
}

void kill_zombies() {
    if (zombie_task) {
        free_user_memory(zombie_task->pd);
        free_kernel_page(zombie_task->pd);
        free_kernel_page(zombie_task->kernel_stack);
        kfree(zombie_task);
        zombie_task = NULL;
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

static uint32_t get_next_pid() {
    return ++pid_counter;
}

static void restart_quantum(task_t *task) {
    task->rem_quantum = task->quantum;
}

/* Crea una nueva tarea lista para ser ejecutada.
 * - ``pd`` es la direccion virtual del directorio de paginas de la tarea;;
 */
task_t *create_task(uint32_t pd[], struct program_t *prog) {

    task_t *task = kmalloc(sizeof(task_t));

    task->prog = prog;

    task->pd = pd;

    task->waiting = FALSE;
    task->waited = FALSE;
    task->parent = NULL;

    task->ticks = 0;
    task->quantum = SCHED_QUANTUM;
    restart_quantum(task);
    
    task->pid = get_next_pid();

    // Alojamos memoria para el stack del kernel de la tarea
    task->kernel_stack = new_kernel_page();
    task->kernel_stack_limit = task->kernel_stack + PAGE_SIZE;
    task->kernel_stack_pointer = task->kernel_stack_limit;

    // Escribimos el task_state_t en la pila del kernel
    void *stack_pointer = elf_stack_bottom(prog->file);
    void *start_task_routine = START_TASK_VIRT_ADDR;
    task->kernel_stack_pointer -= sizeof(task_state_t);
    task_state_t *st = (task_state_t *)task->kernel_stack_pointer;
    // El stack pointer arranca en 2 posiciones abajo del tope. Una es para
    // la direccion de retorno a la que start_task deberia volver. La otra es
    // para el parametro de start_task, que sera justamente el punto de
    // entrada de la tarea.
    initialize_task_state(st, start_task_routine, stack_pointer - 8);

    // Direccion del task_t correspondiente a la tarea
    task->kernel_stack_pointer -= 4;
    *((void **)task->kernel_stack_pointer) = task;
    // Direccion de la rutina que inicializa la tarea
    task->kernel_stack_pointer -= 4;
    *((void **)task->kernel_stack_pointer) = initialize_task;

    // El valor de esta entrada en el stack no deberia tener ninguna
    // importancia. Es el valor que adquiere ebp al cambiar el contexto al
    // de la tarea nueva. Este registro no es usado hasta que adquiere el valor
    // que indica el task_state_t inicial de la tarea.
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

    task_t *current_candidate = old_task->next;
    task_t *first_candidate = current_candidate;
    task_t *selected = NULL;
    // Buscamos una tarea que este lista para ejecutarse. Si no hay ninguna,
    // hacemos halt hasta que ocurra una interrupcion.
    do {
        do {
            if (!(current_candidate->waiting)) {
                selected = current_candidate;
                break;
            }
            current_candidate = current_candidate->next;
        } while(current_candidate != first_candidate);
        
        if (selected == NULL) {
            outb(PIC1_DATA, (PIC_ALL_ENABLED & (~PIC_TIMER)) | PIC_TIMER);
            wait_for_interrupt();
            outb(PIC1_DATA, PIC_ALL_ENABLED);
        }

    } while (selected == NULL);

    task_list = selected;

    switch_context(old_task, current_task());

    restore_eflags(eflags);
}

void switch_if_needed(uint32_t ticks) {
    task_t *current = current_task();
    current->rem_quantum--;
    current->ticks++;

    if (!current->rem_quantum) {
        restart_quantum(current);
        switch_tasks();
    }
}

task_t *get_task_by_pid(uint32_t pid) {
    task_t *first, *curr;
    first = curr = current_task();
    do {
        if (curr->pid == pid)
            return curr;
        curr = curr->next;
    } while(curr != first);
    return NULL;
}

void kill_task(task_t *task) {
    remove_task(task);

    put_zombie(task);

    if (task->waited != FALSE)
        task->parent->waiting = FALSE;

}
