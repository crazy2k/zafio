#
# Configuracion de herramientas
#
CC := gcc
CFLAGS := -std=gnu99 -Wall -nostdlib -nostartfiles -nodefaultlibs -nostdinc

AS := nasm
ASFLAGS := -f elf

LD := ld
LDFLAGS := -T linker.ld

# Linkeo "parcial" de elf files como binary
LDPARTFLAGS := -s -r

#
# Directorios
#
SRCDIR := kernel/src/
INCDIR := kernel/src/
REFTESTDIR := reftest/
OBJSDIR := kernel/obj/
PROGS_DIR := progs/
USER_DIR := user/
USER_PROG_DIRS := $(wildcard $(USER_DIR)/*/Makefile)
USER_PROG_DIRS := $(USER_PROG_DIRS:/Makefile=)

#
# Archivos
#
BUILD_ALL_PROGS := ./buildprogs.sh

PROGS = $(wildcard $(PROGS_DIR)/*)
PROG_OBJS := $(addsuffix .o, $(subst $(PROGS_DIR),$(OBJSDIR),$(PROGS)))

# Codigo C
SOURCES := $(shell find $(SRCDIR) -name "*.c")
HEADERS := $(shell find $(INCDIR) -name "*.h")

OBJS := $(notdir $(SOURCES:.c=.o))
OBJS := $(addprefix $(OBJSDIR), $(OBJS))

# Codigo assembly
ASSOURCES := $(shell find $(SRCDIR) -name "*.S")

ASOBJS := $(notdir $(ASSOURCES:.S=.o))
ASOBJS := $(addprefix $(OBJSDIR), $(ASOBJS))

# Codigo C generado
ALL_PROGS_SOURCE := $(SRCDIR)all_progs.c
ALL_PROGS_OBJ := $(OBJSDIR)all_progs.o

KERNEL := kernel/kernel.bin

# Imagen de disco floppy
DISKETTE := $(REFTESTDIR)/aux/diskette.img


#
# Reglas
#
all: diskette.img

deps: $(SOURCES) $(OBJSDIR) $(PROG_SOURCES) $(PROG_OBJSDIR) $(ALL_PROGS_SOURCE)
	$(CC) $(CFLAGS) -MM $(SOURCES) | sed "s/\(\w*\.o\)/$(subst /,\/, $(OBJSDIR))\1/" > $@
-include deps

$(PROGS_DIR):
	mkdir $(PROGS_DIR)

$(OBJSDIR):
	mkdir $(OBJSDIR)

$(ALL_PROGS_OBJ): $(ALL_PROGS_SOURCE)

$(ALL_PROGS_SOURCE): $(PROGS_DIR) $(BUILD_ALL_PROGS)
	$(BUILD_ALL_PROGS) > $@

$(KERNEL): $(OBJS) $(ASOBJS) $(PROGS_DIR) $(PROG_OBJS) $(ALL_PROGS_OBJ)
	$(LD) $(LDFLAGS) $(ASOBJS) $(OBJS) $(PROG_OBJS) -o $@

$(ASOBJS): $(ASSOURCES)
	$(AS) $(ASFLAGS) $(shell find $(SRCDIR) -name "$(notdir $(patsubst %.o, %.S, $@))") -o $@

$(OBJSDIR)%.o:
	$(CC) $(CFLAGS) -c $< -o $@

install_user_progs:
	for DIR in $(USER_PROG_DIRS); do    \
		$(MAKE) -C $$DIR install;       \
	done

$(PROG_OBJS): install_user_progs
	$(LD) $(LDPARTFLAGS) -b binary $(patsubst $(OBJSDIR)/%.o,$(PROGS_DIR)/%,$@) -o $@

.PHONY: clean new diskette.img all

clean:
	rm -f $(ALL_PROGS_SOURCE)	
	rm -f $(OBJSDIR)/*.o
	rm -f $(KERNEL)
	rm -f deps
	rm -f $(REFTESTDIR)/diskette.img
	for DIR in $(USER_PROG_DIRS); do    \
		$(MAKE) -C $$DIR clean;         \
	done

new: 
	make clean 
	make $(REFTESTDIR)/diskette.img

diskette.img: $(REFTESTDIR)/diskette.img

$(REFTESTDIR)/diskette.img: $(KERNEL) $(DISKETTE)
	cp $(DISKETTE) $(REFTESTDIR)/diskette.img
	mcopy -i $(REFTESTDIR)/diskette.img $(KERNEL) ::/
