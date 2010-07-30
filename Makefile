#
# Configuracion de herramientas
#
CC := gcc
CFLAGS := -std=gnu99 -Wall -Werror -nostdlib -nostartfiles -nodefaultlibs 

AS := nasm
ASFLAGS := -f elf 

LD := ld
LDFLAGS := -T linker.ld

#
# Directorios
#

SRCDIR := src/
REFTESTDIR := reftest/
OBJSDIR := obj/
#USRCDIR := usr/src/
#UOBJSDIR := usr/obj/

#
# Archivos
#

SOURCES := $(shell find $(SRCDIR) -name "*.c")
HEADERS := $(shell find $(SRCDIR) -name "*.h")

OBJS := $(notdir $(SOURCES:.c=.o))
OBJS := $(addprefix $(OBJSDIR), $(OBJS))

KERNEL := kernel.bin

LOADER_SRC := $(SRCDIR)/kernel/loader.S
LOADER_OBJ := $(OBJSDIR)/loader.o

# Imagen de disco floppy
DISKETTE := $(REFTESTDIR)/aux/diskette.img


#
# Reglas
#

$(OBJSDIR):
	mkdir $(OBJSDIR)

$(KERNEL): $(OBJS) $(LOADER_OBJ)
	$(LD) $(LDFLAGS) $(LOADER_OBJ) $(OBJS) -o $@

$(LOADER_OBJ): $(LOADER_SRC)
	$(AS) $(ASFLAGS) $(LOADER_SRC) -o $@

deps: $(SOURCES) $(OBJSDIR)
	$(CC) $(CFLAGS) -MM $(SOURCES) | sed "s/\(\w*\.o\)/$(OBJSDIR:/=\/)\1/" > $@  
-include deps

obj/%.o:
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean new diskette.img

clean:
	rm -f $(OBJSDIR)/*.o
	rm -f $(KERNEL)
	rm -f deps
	rm -f $(REFTESTDIR)/diskette.img

new: clean $(REFTESTDIR)/diskette.img

diskette.img: $(REFTESTDIR)/diskette.img

$(REFTESTDIR)/diskette.img: $(KERNEL) $(DISKETTE)
	cp $(DISKETTE) $(REFTESTDIR)/diskette.img
	mcopy -i $(REFTESTDIR)/diskette.img $(KERNEL) ::/
