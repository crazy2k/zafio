#include "../inc/memlayout.h"
#include "../inc/utils.h"
#include "../inc/io.h"

#define SCREEN_ROWS 25
#define SCREEN_COLS 80
#define SCREEN_CHAR_SIZE 2
#define SCREEN_ROW_SIZE (SCREEN_COLS*SCREEN_CHAR_SIZE)
#define SCREEN_BEGIN VIDEO_MEMORY
#define SCREEN_END (VIDEO_MEMORY + SCREEN_ROWS*SCREEN_COLS*SCREEN_CHAR_SIZE)

#define SCREEN_LINE_BEGIN(pos) \
    (void *)(((((unsigned int)pos - SCREEN_BEGIN)/SCREEN_ROW_SIZE)*\
    SCREEN_ROW_SIZE) + SCREEN_BEGIN)

void *current_pos = (void *)SCREEN_BEGIN;

/*
 * Levanta los datos de la pantalla y ``current_pos`` una linea.
 */
void scroll_down() {
    memcpy((void *)SCREEN_BEGIN, (void *)SCREEN_BEGIN + SCREEN_ROW_SIZE,
        (SCREEN_END - SCREEN_BEGIN) - SCREEN_ROW_SIZE);

    current_pos -= SCREEN_ROW_SIZE;
    clline(current_pos);
}

/*
 * Limpia la linea correspondiente a la posicion ``pos``.
 */
void clline(void *pos) {
    void *begin = SCREEN_LINE_BEGIN(pos);
    int i;
    for (i = 0; i < SCREEN_ROW_SIZE; i++)
        putchr(begin + i, 0);
}

/*
 * Limpia toda la pantalla.
 */
void cls() {
    void *begin = (void *)SCREEN_BEGIN;
    int i;
    for (i = 0; i < SCREEN_ROWS; i++)
        clline(begin + SCREEN_ROW_SIZE*i);
}

/*
 * Imprime la cadena en la pantalla y salta de linea.
 */
void print(char *str) {
    writestr(str);
    writechr('\n');
}

/*
 * Pone el caracter ``chr`` en la posicion ``pos`` de la pantalla.
 */
void putchr(void *pos, char chr) {
    char *bytepos = (char *)pos;
    bytepos[0] = chr;
    bytepos[1] = 0x0F;
}

/*
 * Escribe el caracter ``chr`` en la pantalla y avanza ``current_pos``.
 */
void writechr(char chr) {
    if (chr == '\n')
        current_pos = SCREEN_LINE_BEGIN(current_pos) + SCREEN_ROW_SIZE;
    else {
        if (current_pos >= (void *)SCREEN_END)
            scroll_down();

        putchr(current_pos, chr);

        current_pos += SCREEN_CHAR_SIZE;
    }
}

