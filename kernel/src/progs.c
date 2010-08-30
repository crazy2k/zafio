#include "../inc/progs.h"
#include "../inc/elf_helpers.h"
#include "../inc/mmu.h"
#include "../inc/vmm.h"
#include "../inc/debug.h"


void load_task_image(task_t * task) {
    load_elf_proc_image(task->prog->file, task->pd); 
}

void load_task_stack(task_t* task) {
    void *stack_page = ALIGN_TO_PAGE(elf_stack_bottom(task->prog->file), TRUE) -
        PAGE_SIZE;
    new_page(task->pd, stack_page, STACK_PAGE_FLAGS);
}

void push_entry_point(task_t *task) {
    void *stack_bottom = elf_stack_bottom(task->prog->file);
    *((void **)stack_bottom - 4) = elf_entry_point(task->prog->file);
}

void start_task(int (*main)()) {
    BOCHS_BREAK;

    main();

    // Llamada al sistema 1 (exit)
    __asm__ __volatile__("mov %0, %%eax" : : "i" (1));
    // Llamar
    __asm__ __volatile__("int $0x80" : :);

}
