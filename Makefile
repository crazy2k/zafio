CC := gcc
CFLAGS := -Wall -nostdlib -nostartfiles -nodefaultlibs

AS := nasm
ASFLAGS := -f elf 

LD := ld
LDFLAGS := -T linker.ld 

SOURCES := $(wildcard src/**/*.c)
HEADERS := $(wildcard src/**/*.h)
OBJS := $(notdir $(SOURCES:.c=.o))
OBJS := $(addprefix obj/, $(OBJS))
OBJ_DIR := obj

KERNEL := kernel.bin

STAGES := stage1 stage2

LOADER_OBJ := loader.o
LOADER_SRC := loader.S

PAD := pad

deps: $(SOURCES)
	$(CC) $(CFLAGS) -MM $(SOURCES) | sed "s/\(\w*\.o\)/obj\/\1/" > $@  
-include deps

clean:
	rm -f ./*.o
	rm -f $(KERNEL)

new: clean $(KERNEL)

$(KERNEL): $(OBJS) $(LOADER_OBJ)
	$(LD) $(LDFLAGS) $(OBJS) $(LOADER_OBJ) -o $@

$(LOADER_OBJ): $(LOADER_SRC)
	$(AS) $(ASFLAGS) $(LOADER_SRC) -o $@

$(PAD):
	dd if=/dev/zero of=$(PAD) bs=1 count=750

diskette.img: $(STAGES) $(PAD) $(KERNEL) Makefile
	cat $(STAGES) $(PAD) $(KERNEL) > $@

