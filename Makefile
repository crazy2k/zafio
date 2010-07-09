SRCDIR := src/

CC := gcc
CFLAGS := -Wall -nostdlib -nostartfiles -nodefaultlibs 

AS := nasm
ASFLAGS := -f elf 

LD := ld
LDFLAGS := -T $(SRCDIR)/kernel/linker.ld 

SOURCES := $(shell find $(SRCDIR) -name "*.c")
HEADERS := $(shell find $(SRCDIR) -name "*.h")

OBJSDIR := obj/
OBJS := $(notdir $(SOURCES:.c=.o))
OBJS := $(addprefix $(OBJSDIR), $(OBJS))

KERNEL := kernel.bin

LOADER_SRC := $(SRCDIR)/kernel/loader.S
LOADER_OBJ := $(OBJSDIR)/loader.o

# Datos para el armado de la imagen de disco floppy
GRUBDIR := reftest/grub/boot/grub/
STAGES := stage1 stage2
STAGES := $(addprefix $(GRUBDIR), $(STAGES))
PAD := pad

$(OBJSDIR):
	mkdir $(OBJSDIR)

$(KERNEL): $(OBJS) $(LOADER_OBJ)
	$(LD) $(LDFLAGS) $(OBJS) $(LOADER_OBJ) -o $@

$(LOADER_OBJ): $(LOADER_SRC)
	$(AS) $(ASFLAGS) $(LOADER_SRC) -o $@

deps: $(SOURCES) $(OBJSDIR)
	$(CC) $(CFLAGS) -MM $(SOURCES) | sed "s/\(\w*\.o\)/$(OBJSDIR:/=\/)\1/" > $@  
-include deps

obj/%.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJSDIR)*.o
	rm -f $(KERNEL)
	rm -f $(PAD)
	rm -f deps
	rm -f diskette.img

new: clean $(KERNEL)

STAGESIZES := $(foreach x, $(STAGES), + $(shell stat -c %s $(x)))
SIZE := $(shell echo $(addsuffix ' +', $(STAGESIZES)) 0 | bc )
PADSIZE := $(shell echo "512 - $(SIZE) % 512" | bc )

$(PAD):
	dd if=/dev/zero of=$(PAD) bs=1 count=$(PADSIZE)

diskette.img: $(STAGES) $(PAD) $(KERNEL) Makefile
	cat $(STAGES) $(PAD) $(KERNEL) > $@
