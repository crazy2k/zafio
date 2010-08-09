#include "../inc/progs.h"


void load_task_image(struct task_t * task) {
    load_elf_proc_image(task->prog->file, task->pd); 
}
