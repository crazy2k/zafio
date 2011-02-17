#include "../inc/vmm.h"
#include "../inc/io.h"
#include "../inc/progs.h"
#include "../inc/debug.h"
#include "../inc/sched.h"
#include "../inc/utils.h"
#include "../inc/syscalls.h"

static void welcome_msg();

char buf[1024];

#define INIT_PROGNAMES_SIZE (sizeof(init_prognames)/sizeof(char *))

char *init_prognames[] = { "shell" };



void init_task() {
    welcome_msg();

    int pid;
    // Creamos los nuevos task_t para las tareas de inicio
    for (int i = 0; i < INIT_PROGNAMES_SIZE; i++) {
        pid = sys_run(init_prognames[i]);
    }

    sys_waitpid(pid);

    BOCHS_BREAK;

    while (1)
        switch_tasks();

}

void welcome_msg() {
    kputs("                    w\n");
    kputs("                  ww         www\n");
    kputs("                www        wwwwww\n");
    kputs("               www       wwwwwwww\n");
    kputs("               www     wwwwwwwww\n");
    kputs("                www  wwwwwwwwww\n");
    kputs("                 wwwwwwwwwwwww\n");
    kputs("                 wwwwwwwww\n");
    kputs("                 www\n");
    kputs("                www\n");
    kputs("               www\n");
    kputs("              www        Bienvenido a Zafio\n");
    kputs("             www\n");
    kputs("             www\n");
    kputs("             www\n");
    kputs("             www\n");
    kputs("              wwww           ww\n");
    kputs("               wwwwwwwwwwwwwwwww\n");
    kputs("                wwwwwwwwwwwwwwwww\n");
    kputs("                  wwwwww     wwwww\n");
    kputs("                               www\n");
    kputs("                               www\n");
    kputs("                         w    ww\n");
    kputs("                        wwwwwwww\n");
    kputs("                         wwwww\n");
}
