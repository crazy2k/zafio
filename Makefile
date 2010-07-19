SRCDIR := src/
REFTESTDIR := reftest/

CC := gcc
CFLAGS := -Wall -nostdlib -nostartfiles -nodefaultlibs 

AS := nasm
ASFLAGS := -f elf 

LD := ld
LDFLAGS := -T linker.ld

SOURCES := $(shell find $(SRCDIR) -name "*.c")
HEADERS := $(shell find $(SRCDIR) -name "*.h")

OBJSDIR := obj/
OBJS := $(notdir $(SOURCES:.c=.o))
OBJS := $(addprefix $(OBJSDIR), $(OBJS))

KERNEL := kernel.bin

ENTRY_SRC := $(SRCDIR)/kernel/entry.S
ENTRY_OBJ := $(OBJSDIR)/entry.o

STARTER_SRC := $(SRCDIR)/kernel/starter.S
STARTER_OBJ := $(OBJSDIR)/starter.o

# Datos para el armado de la imagen de disco floppy
DISKETTE := $(REFTESTDIR)/aux/diskette.img

$(OBJSDIR):
	mkdir $(OBJSDIR)

$(KERNEL): $(OBJS) $(STARTER_OBJ) $(ENTRY_OBJ)
	$(LD) $(LDFLAGS) $(ENTRY_OBJ) $(OBJS) -o $@

$(ENTRY_OBJ): $(ENTRY_SRC)
	$(AS) $(ASFLAGS) $(ENTRY_SRC) -o $@

$(STARTER_OBJ): $(STARTER_SRC)
	$(AS) $(ASFLAGS) $(STARTER_SRC) -o $@

deps: $(SOURCES) $(OBJSDIR)
	$(CC) $(CFLAGS) -MM $(SOURCES) | sed "s/\(\w*\.o\)/$(OBJSDIR:/=\/)\1/" > $@  
-include deps

obj/%.o:
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean new diskette.img

clean:
	rm -f $(OBJSDIR)*.o
	rm -f $(KERNEL)
	rm -f deps
	rm -f $(REFTESTDIR)/diskette.img

new: clean $(KERNEL)

diskette.img: $(REFTESTDIR)/diskette.img

$(REFTESTDIR)/diskette.img: $(KERNEL) $(DISKETTE)
	cp $(DISKETTE) $(REFTESTDIR)/diskette.img
	mcopy -i $(REFTESTDIR)/diskette.img $(KERNEL) ::/
