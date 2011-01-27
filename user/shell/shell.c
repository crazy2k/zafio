#include <types.h>
#include <io.h>
#include <utils.h>

#define BUFF_LEN 80

#define HELP "help"
#define ECHO "echo"
#define PS "ps"
#define LS "ls"
#define REBOOT "reboot"
#define RUN "run"
#define RUN_BG "bg"


char line_buffer[BUFF_LEN];

static void command_use_error();
static void print_shell_use();
static char *skip_spaces(char* str);
static char *get_word(char** str);

int main() {
    int count;
    char *rest, *command_str, *param_str;

    //Inicializar terminal... 

    while(TRUE) {
        count = read_line(line_buffer, BUFF_LEN);

        if (count == -1) {
            while (read_line(line_buffer, BUFF_LEN) != 1);

            write_line("Line too long");
        } else {
            line_buffer[count -1] = NULL;
            rest = skip_spaces(line_buffer);

            if (!*rest) {
                command_use_error();
                continue;
            }

            command_str = get_word(&rest);
            rest = skip_spaces(rest);
            param_str = get_word(&rest);

            if (strcmp(HELP, command_str) == 0) {
                print_shell_use();
            } else if (strcmp(ECHO, command_str) == 0) {
                write_line(param_str);
            } else if (strcmp(PS, command_str) == 0) {
                //Imprimir informacion de programas en ejecucion
            } else if (strcmp(LS, command_str) == 0) {
                //Imprimir lista de programas disponibles
            } else if (strcmp(REBOOT, command_str) == 0) {
                //Resetear
            } else if (strcmp(RUN, command_str) == 0) {
                //Ejecutar un programa en foreground
            } else if (strcmp(RUN_BG, command_str) == 0) {
                //Ejecutar un programa en background
            } else
                command_use_error();
        }
    }

   return 0;
}

// Se saltea todos los espacios y retorna el primer elemento que no es un espacio
char *skip_spaces(char* str) {
    while (*str == ' ') str++;
    return str;
}

/* Deja apuntando en el resultado la siguiente palabra q encuentra, 
   pone un NULL al final de la misma y deja en str la posicion siguente 
   a donde dejo el NULL*/
char *get_word(char** str) {
    char *result = *str, 
        *curr = *str;

    while (*curr != NULL && *curr != ' ')
        curr++;

    if (*curr != NULL ) {
        *curr = NULL;
        *str = ++curr;
    }

    return result;
}
       
void command_use_error() {
    //Comando erroneo
    
    print_shell_use();
}

void print_shell_use() {
    write_line("Ayudaaa!!!");
}
