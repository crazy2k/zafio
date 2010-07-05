# ensamblar loader.s
nasm -f elf -o loader.o loader.s
# compilar kernel.c
gcc -o kernel.o -c kernel.c -Wall -nostdlib -nostartfiles -nodefaultlibs
# linkear ambos en kernel.bin
ld -T linker.ld -o kernel.bin loader.o kernel.o
# crear archivo de padding de 750 bytes (esto es para que el kernel
# este a los 102400 bytes); hay que cambiarlo si se cambian los stages
dd if=/dev/zero of=pad bs=1 count=750
# armar el diskette
cat stage1 stage2 pad kernel.bin > diskette.img

# en bochs, una vez que carga grub hay que escribir:
# > kernel 200+10
# > boot
