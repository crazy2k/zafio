#include "../inc/mmu.h"
#include "../inc/vmmu.h"
#include "../inc/io.h"

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
    kputui32((uint32_t)memory_info.first);
    kputs(" - corresponde a la pagina ");
    kputui32((uint32_t)PAGE_TO_PHADDR(memory_info.first));
    kputs("\n");

    kputs("Previo al primer 'page': ");
    kputui32((uint32_t)memory_info.first->prev);
    kputs(" - corresponde a la pagina ");
    kputui32((uint32_t)PAGE_TO_PHADDR(memory_info.first->prev));
    kputs("\n");

    kputs("Ultimo 'page'          : ");
    kputui32((uint32_t)memory_info.last);
    kputs(" - corresponde a la pagina ");
    kputui32((uint32_t)PAGE_TO_PHADDR(memory_info.last));
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
    page_t *first = memory_info.first;
    print_page(0, first);

    page_t *curr_page;
    int i;
    for (i = 1, curr_page = first->next;
        curr_page != first;
        i++, curr_page = curr_page->next) {

        print_page(i, curr_page);
    }
}


void cmain() {
    kcls();
    welcome_msg();
    debug_prints();

    // Mapeamos las paginas en las que se encuentran los page_t
    void *start = kpage_align((void *)memory_info.first, 0);
    void *end = kpage_align((void *)memory_info.last, 1);
    int n = ((uint32_t)(end - start))/PAGE_SIZE;
    map_kernel_pages(kernel_pd, start, n);

    // Quitamos el mapeo de los primeros 4MB del espacio de direcciones virtual
    page_dir_unmap(kernel_pd, (void *)0x00000000);

    print_pages();

    // Si no ejecutamos un hlt aca, hay un page fault, ya que el codigo que nos
    // llamo ya no esta mapeado en el espacio de memoria virtual
    __asm__ __volatile__("hlt");

}


