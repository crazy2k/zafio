#include "../inc/progs.h"
#include "../inc/elf_helpers.h"
#include "../inc/mmu.h"
#include "../inc/vmm.h"


void load_task_image(task_t * task) {
    load_elf_proc_image(task->prog->file, task->pd); 
}

void load_task_stack(task_t* task) {
    void *stack_page = ALIGN_TO_PAGE(elf_stack_bottom(task->prog->file), FALSE) -
        PAGE_SIZE;
    new_page(task->pd, stack_page, STACK_PAGE_FLAGS);
}
