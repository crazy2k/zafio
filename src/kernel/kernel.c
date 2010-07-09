
void cmain(unsigned int magic_number, void* mbinfo) {
    if (magic_number != 0x1BADB002) {
        // no nos cargo un bootloader Multiboot: mbinfo no sirve y no
        // conocemos el estado de la maquina con precision
    }

    unsigned char *video_mem = (unsigned char *) 0xB8000;
    int i;
    for (i = 0; i < 80*25*2; i++) {
        video_mem[i] = 0;
    }


}
