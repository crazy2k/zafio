
gcc -nostdlib -nostdinc -nostartfiles -nodefaultlibs -c sec.c 
ld -e main sec.o -o sec
