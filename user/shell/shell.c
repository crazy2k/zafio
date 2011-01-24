#include <types.h>

#define BUFF_LEN (80)
#define TERM_INPUT (0)


char line_buffer[BUFF_LEN];

static void command_error();
static print_shell_use();
static char *skip_spaces(char* str);
static char *get_word(char** result, char* str);

int main() {
    int readed;
    char *rest, *command_str, *param_str;

    //Inicializar terminal... 

    while(TRUE) {
        readed = read(TERM_INPUT, line_buffer, BUFF_LEN);

        if (readed == -1)
            user_error("Line too long")
        else {
            buff[readed] = NULL;

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
                print_shell_use();
            
        }
    }

   return 0;
}

//Se salte todos los espacios y retorna el primer elemento sin espacios
char *skip_spaces(char* str) {
    while (*str == ' ') str++;
    return str;
}

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

void command_error() {
    //Comando erroo
    
    print_shell_use();
}

void print_shell_use() {
}
