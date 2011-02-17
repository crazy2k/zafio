#include <types.h>
#include <io.h>
#include <utils.h>
#include <sched.h>

#define BUFF_LEN 80
#define RESULT_BUF_LEN 1024

#define PROMPT "$ "
#define HELP "help"
#define ECHO "echo"
#define PS "ps"
#define LS "ls"
#define RUN "run"
#define RUN_BG "bg"
#define NICE "nice"
#define KILL "kill"


char line_buffer[BUFF_LEN];
char result_buf[RESULT_BUF_LEN];

static void command_use_error();
static void print_shell_use();
static void print_ps(char *mode);
static void print_ls(char *mode);
static void do_run(char *progname);
static void do_runbg(char *progname);
static char *skip_spaces(char* str);
static char *get_word(char** str);
static void do_nice(char *process, char *value);
static void do_kill(char *pid);

int main() {
    int count;
    char *rest, *command_str, *param_str, *param2_str;

    //Inicializar terminal... 

    devreq(KEYBOARD);
    devreq(SCREEN);
    devreq(TERMINAL);

    while(TRUE) {
        write(TERMINAL, PROMPT, strlen(PROMPT));
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
            param2_str = get_word(&rest);

            if (strcmp(HELP, command_str) == 0) {
                print_shell_use();
            } else if (strcmp(ECHO, command_str) == 0) {
                write_line(param_str);
            } else if (strcmp(PS, command_str) == 0) {
                //Imprimir informacion de programas en ejecucion
                print_ps(param_str);
            } else if (strcmp(LS, command_str) == 0) {
                //Imprimir lista de programas disponibles
                print_ls(param_str);
            } else if (strcmp(RUN, command_str) == 0) {
                //Ejecutar un programa en foreground
                do_run(param_str);
            } else if (strcmp(RUN_BG, command_str) == 0) {
                //Ejecutar un programa en background
                do_runbg(param_str);
            } else if (strcmp(NICE, command_str) == 0) {
                //Cambiar prioridad de un proceso
                do_nice(param_str, param2_str);
            } else if (strcmp(KILL, command_str) == 0) {
                //Ejecutar un programa en background
                do_kill(param_str);
            }  else
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
        ++curr;
    }
    *str = curr;

    return result;
}

static void print_ps(char *mode) {
    if (strlen(mode) == 0) {
        int n = ps(0, result_buf, RESULT_BUF_LEN);
        result_buf[n] = '\0';

        write_line(result_buf);
    }
}

static void print_ls(char *mode) {
    if (strlen(mode) == 0) {
        int n = ls(0, result_buf, RESULT_BUF_LEN);
        result_buf[n] = '\0';

        write_line(result_buf);
    }
}

static void do_run(char *progname) {
    devrel(KEYBOARD);
    devrel(SCREEN);
    devrel(TERMINAL);

    int pid = run(progname);
    waitpid(pid);

    devreq(KEYBOARD);
    devreq(SCREEN);
    devreq(TERMINAL);

}

static void do_runbg(char *progname) {
    run(progname);
}

static void do_nice(char *process, char *value) {
    nice(strtoi(process), strtoi(value));
}

static void do_kill(char *pid) {
    kill(strtoi(pid));
}

void command_use_error() {
    //Comando erroneo
    
    print_shell_use();
}

void print_shell_use() {
    write_line("Comandos disponibles:");
    write_line("help                muestra esta ayuda");
    write_line("ls                  muestra la lista de programas disponibles");
    write_line("ps                  muestra datos sobre los procesos del sistema");
    write_line("run <programa>      ejecuta un programa en foreground");
    write_line("bg <programa>       ejecuta un programa en background");
    write_line("nice <pid> <valor>  cambia la prioridad de un proceso");
    write_line("kill <pid>          mata un proceso");
    write_line("echo <palabra>      imprime una palabra");
}
