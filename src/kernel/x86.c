#include "../inc/x86.h"

void outb(uint32_t port, char value) {
    __asm__ __volatile__("outb %b0, %w1" : : "r" (value), "r" (port));
}

char inb(uint32_t port) {
    char value;
    __asm__ __volatile__("inb %w1, %b0" : "=a" (value) : "d" (port));
    return value;
}

void lgdt(gdtr_t gdtr) {
    __asm__ __volatile__ ("lgdt %0" : : "m" (gdtr));
}

void lidt(idtr_t idtr) {
    __asm__ __volatile__ ("lidt %0" : : "m" (idtr));
}

void ltr(uint16_t segsel) {
    __asm__ __volatile__ ("ltr %0" : : "r" (segsel));
}

uint32_t disable_interrupts() {
    uint32_t eflags;
    __asm__ __volatile__("pushf");
    __asm__ __volatile__("pop %0" : "=a" (eflags) :);
    __asm__ __volatile__("cli");

    return eflags;
}

void restore_eflags(uint32_t eflags) {
    __asm__ __volatile__("push %0" : : "r" (eflags));
    __asm__ __volatile__("popf");
}



void sti() {
    __asm__ __volatile__ ("sti");
}

void load_cr3(uint32_t pd) {
    __asm__ __volatile__("mov %0, %%cr3" : : "r" (pd));
}
