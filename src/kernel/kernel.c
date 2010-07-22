#include "../inc/mmu.h"
#include "../inc/vmmu.h"
#include "../inc/io.h"

void cmain() {
    kcls();
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

    kputs("Primer 'page': ");
    kputui32((uint32_t)memory_info.first);
    kputs(" - corresponde a la pagina ");
    kputui32((uint32_t)PAGE_TO_PHADDR(memory_info.first));
    kputs("\n");

    kputs("Ultimo 'page': ");
    kputui32((uint32_t)memory_info.last);
    kputs(" - corresponde a la pagina ");
    kputui32((uint32_t)PAGE_TO_PHADDR(memory_info.last));
    kputs("\n");



    kputs("\n");
}
