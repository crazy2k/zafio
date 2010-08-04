#
# Configuracion de herramientas
#
CC := gcc
CFLAGS := -std=gnu99 -Wall -Werror -nostdlib -nostartfiles -nodefaultlibs -nostdinc  

AS := nasm
ASFLAGS := -f elf 

LD := ld
LDFLAGS := -T linker.ld

# Linkeo "parcial" de elf files como binary
LDPARTFLAGS := -s -r 

#
# Directorios
#

SRCDIR := src/
REFTESTDIR := reftest/
OBJSDIR := obj/
PROGS_DIR := progs/

#
# Archivos
#
SOURCES := $(shell find $(SRCDIR)/kernel -name "*.c")
HEADERS := $(shell find $(SRCDIR)/kernel -name "*.h")

OBJS := $(notdir $(SOURCES:.c=.o))
OBJS := $(addprefix $(OBJSDIR), $(OBJS))

PROGS := $(wildcard $(PROGS_DIR)/*)
PROG_OBJS := $(addsuffix .o, $(subst $(PROGS_DIR),$(OBJSDIR),$(PROGS)))

KERNEL := kernel.bin

LOADER_SRC := $(SRCDIR)/kernel/loader.S
LOADER_OBJ := $(OBJSDIR)/loader.o

# Imagen de disco floppy
DISKETTE := $(REFTESTDIR)/aux/diskette.img


#
# Reglas
#

all: diskette.img

$(PROGS_DIR):
	mkdir $(PROGS_DIR)

$(OBJSDIR):
	mkdir $(OBJSDIR)

$(KERNEL): $(OBJS) $(LOADER_OBJ) $(PROGS_DIR) $(PROG_OBJS)
	$(LD) $(LDFLAGS) $(LOADER_OBJ) $(OBJS) $(PROG_OBJS) -o $@

$(LOADER_OBJ): $(LOADER_SRC)
	$(AS) $(ASFLAGS) $(LOADER_SRC) -o $@

deps: $(SOURCES) $(OBJSDIR) $(PROG_SOURCES) $(PROG_OBJSDIR)
	$(CC) $(CFLAGS) -MM $(SOURCES) | sed "s/\(\w*\.o\)/$(OBJSDIR:/=\/)\1/" > $@  
-include deps

$(OBJSDIR)%.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(PROG_OBJS):
	$(LD) $(LDPARTFLAGS) -b binary $(patsubst $(OBJSDIR)/%.o,$(PROGS_DIR)/%,$@) -o $@


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
