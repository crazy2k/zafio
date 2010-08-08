#include "../inc/io.h"
#include "../inc/vmm.h"
#include "../inc/idt.h"
#include "../inc/sched.h"


void cmain() {
    kcls();

    vm_init();
    idt_init();
    sched_init();

    // Si no ejecutamos un hlt aca, hay un page fault, ya que el codigo que nos
    // llamo ya no esta mapeado en el espacio de memoria virtual
    __asm__ __volatile__("hlt");

}


