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

# Codigo C
SOURCES := $(shell find $(SRCDIR) -name "*.c")
HEADERS := $(shell find $(SRCDIR) -name "*.h")
OBJS := $(notdir $(SOURCES:.c=.o))
OBJS := $(addprefix $(OBJSDIR), $(OBJS))

# Codigo assembly
ASSOURCES := $(shell find $(SRCDIR) -name "*.S")
ASOBJS := $(notdir $(ASSOURCES:.S=.o))
ASOBJS := $(addprefix $(OBJSDIR), $(ASOBJS))

KERNEL := kernel.bin

# Imagen de disco floppy
DISKETTE := $(REFTESTDIR)/aux/diskette.img


#
# Reglas
#

$(OBJSDIR):
	mkdir $(OBJSDIR)

$(KERNEL): $(OBJS) $(ASOBJS)
	$(LD) $(LDFLAGS) $(ASOBJS) $(OBJS) -o $@

$(ASOBJS): $(ASSOURCES)
	$(AS) $(ASFLAGS) $< -o $@

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
