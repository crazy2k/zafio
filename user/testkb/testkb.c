#include <types.h>
#include <syscalls.h>
#include <io.h>

char buf[1024] = { 0 };
char *msg = "Holasss!\n";

int main() {
    while (1) {
        //write(1, msg, 1024);
        int n = read(2, buf, 1024);

        buf[n] = '\0';
        if (n > 0) {
            write(1, buf, 1024);
        }
    }

    return 0;
}

