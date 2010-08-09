#include "../inc/vmm.h"
#include "../inc/io.h"
#include "../inc/progs.h"
#include "../inc/debug.h"
#include "../inc/sched.h"

static void welcome_msg();

void init_task() {
    welcome_msg();

    // Creamos los nuevos task_t para las tareas
    for (int i = 0; i < programs_size; i++) {
        uint32_t *pd = clone_pd(kernel_pd);
        add_task(create_task(pd, &programs[i]));
    }
    BOCHS_BREAK;

    while(1)
        ;

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
