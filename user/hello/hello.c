char *val = "Hola!\n";

int main() {
    for (int i = 0; i < 9; i++) {
        //__asm__ __volatile__("xchg %bx, %bx");

        // Llamada al sistema 4
        __asm__ __volatile__("mov %0, %%eax" : : "i" (4));
        // Primer parametro
        __asm__ __volatile__("mov %0, %%ebx" : : "m" (val));
        // Llamar
        __asm__ __volatile__("int $0x80" : :);
    }
    return 0;
}
