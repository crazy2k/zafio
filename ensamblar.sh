nasm -f bin "$1.asm" -o kernel.bin
# Copiamos el archivo al diskette
mcopy -i diskette.img kernel.bin ::/
