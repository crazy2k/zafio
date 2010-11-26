#include "../inc/progs.h"
#include "../inc/elf_helpers.h"
#include "../inc/mmu.h"
#include "../inc/vmm.h"


void load_task_image(task_t * task) {
    load_elf_proc_image(task->prog->file, task->pd); 
}

void load_task_stack(task_t* task) {
    void *stack_page = ALIGN_TO_PAGE(elf_stack_bottom(task->prog->file), TRUE) -
        PAGE_SIZE;
    new_page(task->pd, stack_page, STACK_PAGE_FLAGS);
}

//Liberar memoria en el espacio de usuario del page directory en pd
void free_user_memory(uint32_t pd[]) {
    for (int i = 0; i < 0x300) {
        if (pd[i] | PDE_P) {
            for (int j = 0; j < 0x400) {
                void *dir (void*) (PAGE_4MB_SIZE) + (PAGE_SIZE) * j;
                free_page(pd,dir);
            }
        }
    }
}
