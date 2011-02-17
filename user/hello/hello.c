#include <io.h>

char line_buffer[1024];

int main() {
    devreq(KEYBOARD);
    devreq(SCREEN);
    devreq(TERMINAL);

    write(TERMINAL, "Nombre: ", 9);

    read_line(line_buffer, 1024);

    write(TERMINAL, "Hola ", 6);
    write(TERMINAL, line_buffer, 1024);

    devrel(KEYBOARD);
    devrel(SCREEN);
    devrel(TERMINAL);

    return 0;
}

