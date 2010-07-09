CC=gcc
AS=nasm
AFLAGS=-f elf 
CFLAGS=-Wall -nostdlib -nostartfiles -nodefaultlibs
LD=ld
LDFLAGS=-T linker.ld 
OBJS=$(SOURCES:.c=.o)
SOURCES=$(wildcard *.c)
HEADERS=$(wildcard *.h)
KERNEL=kernel.bin
STAGES=stage1 stage2
LOADER_OBJ=loader.o
LOADER_SRC=loader.S
PAD=pad

$(KERNEL): $(OBJS) $(LOADER_OBJ)
	$(LD) $(LDFLAGS) $(OBJS) $(LOADER_OBJ) -o $@

$(LOADER_OBJ): $(LOADER_SRC)
	$(AS) $(AFLAGS) $(LOADER_SRC) -o $@

deps: $(SOURCES)
	$(CC) $(CFLAGS) -MM $(SOURCES) > $@  
-include deps

clean:
	rm -f ./*.o
	rm -f $(KERNEL)

new: clean $(KERNEL)

$(PAD): 
  dd if=/dev/zero of=pad bs=1 count=750

diskette: $(STAGES) $(PAD) $(KERNEL) Makefile
  cat stage1 stage2 pad kernel.bin > diskette.img

