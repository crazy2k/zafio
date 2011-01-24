#include <types.h>

#define BUFF_LEN (80)
#define TERM_INPUT (0)


char line_buffer[BUFF_LEN];

static void command_error();
static void user_error(char *msg);
static void print_shell_use();
static char *skip_spaces(char* str);
static char *get_word(char** result, char* str);

int main() {
    int readed;
    char *rest, *command_str, *param_str;

    //Inicializar terminal... 

    while(TRUE) {
        readed = read(TERM_INPUT, line_buffer, BUFF_LEN);

        if (readed == -1)
            user_error("Line too long");
        else {
            line_buffer[readed] = NULL;

            rest = skip_spaces(line_buffer);

            if (!rest) command_error();

            rest = get_word(&command_str ,rest);

            rest = skip_spaces(rest);

            rest = get_word(&param_str, rest);

            if (strcmp(command_str,ALGUN_COMADO) == 0) {
                //Hacer cosas...
            } else if (strcmp(command_str,OTRO_COMADO) == 0) {
                //Hacer otras cosas...
            } else
                command_error();
            
        }
    }

   return 0;
}

// Se saltea todos los espacios y retorna el primer elemento que no es un espacio
char *skip_spaces(char* str) {
    while (*str == ' ') str++;
    return str;
}

/* Deja apuntando a result a la siguiente palabra q encuentra, 
   pone un NULL al final de la misma y retorna la posicion siguente 
   a donde dejo el NULL*/
char *get_word(char** result, char* str) {
    *result = str;

    while (*str != NULL && *str != ' ')
        str++;

    if (*str != NULL ) {
        *str = NULL;
        str++;
    }

    return str;
}

void user_error(char *msg) {

}
        
void command_error() {
    //Comando erroneo
    
    print_shell_use();
}

void print_shell_use() {

}
