CC := gcc
CFLAGS := -Wall -nostdlib -nostartfiles -nodefaultlibs

AS := nasm
ASFLAGS := -f elf 

LD := ld
LDFLAGS := -T ./src/kernel/linker.ld 

SRCDIR := ./src/
SOURCES := $(shell find $(SRCDIR) -name "*.c")
HEADERS := $(shell find $(SRCDIR) -name "*.h")

OBJSDIR := ./obj/
OBJS := $(notdir $(SOURCES:.c=.o))
OBJS := $(addprefix $(OBJSDIR), $(OBJS))

KERNEL := kernel.bin

LOADER_SRC := loader.S
LOADER_OBJ := loader.o

# Datos para el armado de la imagen de disco floppy
GRUBDIR := ./reftest/grub/
STAGES := stage1 stage2
STAGES := $(addprefix $GRUBDIR, $(STAGES))
PAD := ./pad

deps: $(SOURCES)
	$(CC) $(CFLAGS) -MM $(SOURCES) | sed "s/\(\w*\.o\)/obj\/\1/" > $@  
-include deps

clean:
	rm -f $(OBJSDIR)*.o
	rm -f $(KERNEL)
	rm -f $(PAD)
	rm -f deps

new: clean $(KERNEL)

$(KERNEL): $(OBJS) $(LOADER_OBJ)
	$(LD) $(LDFLAGS) $(OBJS) $(LOADER_OBJ) -o $@

$(LOADER_OBJ): $(LOADER_SRC)
	$(AS) $(ASFLAGS) $(LOADER_SRC) -o $@

$(PAD):
	dd if=/dev/zero of=$(PAD) bs=1 count=750

diskette.img: $(STAGES) $(PAD) $(KERNEL) Makefile
	cat $(STAGES) $(PAD) $(KERNEL) > $@

