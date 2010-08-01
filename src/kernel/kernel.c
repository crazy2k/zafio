#include "../inc/mmu.h"
#include "../inc/vmm.h"
#include "../inc/io.h"
#include "../inc/debug.h"
#include "../inc/sched.h"
#include "../inc/idt.h"

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

void debug_prints() {
    kputs("Primer 'page'          : ");
    kputui32((uint32_t)memory_info.first_page);
    kputs(" - corresponde a la pagina ");
    kputui32((uint32_t)PAGE_TO_PHADDR(memory_info.first_page));
    kputs("\n");

    kputs("Previo al primer 'page': ");
    kputui32((uint32_t)memory_info.first_page->prev);
    kputs(" - corresponde a la pagina ");
    kputui32((uint32_t)PAGE_TO_PHADDR(memory_info.first_page->prev));
    kputs("\n");

    kputs("Ultimo 'page'          : ");
    kputui32((uint32_t)memory_info.last_page);
    kputs(" - corresponde a la pagina ");
    kputui32((uint32_t)PAGE_TO_PHADDR(memory_info.last_page));
    kputs("\n");

    kputs("Lower: ");
    kputui32((uint32_t)memory_info.lower);
    kputs("\n");

    kputs("Upper: ");
    kputui32((uint32_t)memory_info.upper);
    kputs("\n");
}

void print_page(int i, page_t *page) {
    kputs("page_t ");
    kputui32((uint32_t)i);
    kputs(": ");
    kputui32((uint32_t)page);
    kputs(" - corresponde a la pagina ");
    kputui32((uint32_t)PAGE_TO_PHADDR(page));
    kputs("\n");
}

// Imprimir lista de page_t
void print_pages() {
    page_t *first_page = memory_info.first_page;
    print_page(0, first_page);

    int i;
    page_t *curr_page;
    for (i = 1, curr_page = first_page->next;
        curr_page != first_page;
        i++, curr_page = curr_page->next) {

        print_page(i, curr_page);
    }
}


void cmain() {
    kcls();

    vm_init();
    idt_init();

    welcome_msg();
    debug_prints();

    tasks_test();

    print_pages();

    // Si no ejecutamos un hlt aca, hay un page fault, ya que el codigo que nos
    // llamo ya no esta mapeado en el espacio de memoria virtual
    __asm__ __volatile__("hlt");

}


