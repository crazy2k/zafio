#include "../inc/memlayout.h"
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

void load_start_task(task_t* task) {
    uint32_t *pt = new_page_table(task->pd, START_TASK_VIRT_ADDR);
    void *start_task_pha = get_kphaddr(ALIGN_TO_PAGE(&start_task, FALSE));
    page_table_map(pt, START_TASK_VIRT_ADDR, start_task_pha, CODE_PAGE_FLAGS);
}

void push_entry_point(task_t *task) {
    void *stack_bottom = elf_stack_bottom(task->prog->file);
    *((void **)(stack_bottom - 4)) = elf_entry_point(task->prog->file);
}

void start_task(int (*main)()) {
    main();

    // Llamada al sistema 1 (exit)
    __asm__ __volatile__("mov %0, %%eax" : : "i" (1));
    // Llamar
    __asm__ __volatile__("int $0x80" : :);

}
