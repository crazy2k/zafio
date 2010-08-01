#include "../inc/debug.h"
#include "../inc/idt.h"
#include "../inc/memlayout.h"
#include "../inc/mmu.h"
#include "../inc/x86.h"

void idt_init() {
    idt[14] = IDT_DESC_SEL(GDT_SEGSEL(0x0, GDT_INDEX_KERNEL_CS)) |
        IDT_DESC_OFFSET(idt_pf_handler) | IDT_DESC_P | IDT_DESC_DPL(0) |
        IDT_DESC_D | IDT_DESC_INT;

    lidt(idtr);

    remap_PIC(PIC1_OFFSET, PIC2_OFFSET);

    sti();
}

void idt_pf_handler() {
    BOCHS_BREAK;
}

void remap_PIC(char offset1, char offset2) {
    // Inicializamos PIC1

    // Se envia ICW4, varios 8259A, interrupt vectors de 8 bytes, interrupciones
    // por flanco e inicializacion (ICW1)
    outb(PIC1_COMMAND, ICW1_INIT + ICW1_ICW4);
    outb(PIC1_DATA, offset1);       // Offset de interrupciones a la CPU (ICW2)
    outb(PIC1_DATA, ICW3_MATTACH2); // El slave ingresa por IRQ2 (ICW3)
    outb(PIC1_DATA, ICW4_8086);     // Modo 8086 (ICW4)

    // Enmascaramos las interrupciones del PIC1
    outb(PIC1_DATA, 0xFF);

    // Inicializamos PIC2

    outb(PIC2_COMMAND, ICW1_INIT + ICW1_ICW4);  // Idem PIC1 (ICW1)
    outb(PIC2_DATA, offset2);       // Offset de interrupciones a la CPU (ICW2)
    outb(PIC2_DATA, ICW3_SATTACH2); // Se ingresa al PIC1 por IRQ2 (ICW3)
    outb(PIC2_DATA, ICW4_8086);     // Modo 8086 (ICW4)
    
    // Enmascaramos las interrupciones del PIC2
    outb(PIC2_DATA, 0xFF);
}
