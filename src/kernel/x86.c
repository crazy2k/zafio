
#include "../inc/x86.h"

void outb(uint32_t port, char value) {
    __asm__ __volatile__("outb %b0, %w1" : : "r" (value), "r" (port));
}

char inb(uint32_t port) {
    char value;
    __asm__ __volatile__("inb %w1, %b0" : "=a" (value) : "d" (port));
    return value;
}
void lidt(idtr_t idtr) {
    __asm__ __volatile__ ("lidt %0" : : "m" (idtr));
}

void sti() {
    __asm__ __volatile__ ("sti");
}
