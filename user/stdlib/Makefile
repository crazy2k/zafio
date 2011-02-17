INCLUDE := inc/

CC := gcc
CFLAGS := -std=gnu99 -Wall -nostdlib -nostdinc -nostartfiles -nodefaultlibs -I$(INCLUDE)

AR := ar
ARFLAGS := -cvq

LIBDIR := ../lib
OBJSDIR := obj/
SRCDIR := src/
LIBNAME := zafiostd
LIBFILE := $(LIBDIR)/lib$(LIBNAME).a

SOURCES := $(wildcard $(SRCDIR)/*.c)
HEADERS := $(wildcard $(SRCDIR)/*.h)

OBJS := $(notdir $(SOURCES:.c=.o))
OBJS := $(addprefix $(OBJSDIR), $(OBJS))

all: $(LIBFILE)

$(LIBDIR):
	mkdir $(LIBDIR)

$(OBJSDIR):
	mkdir $(OBJSDIR)

install: $(LIBFILE)

deps: $(SOURCES) $(OBJSDIR)
	$(CC) $(CFLAGS) -MM $(SOURCES) | sed "s/\(\w*\.o\)/$(subst /,\/, $(OBJSDIR))\1/" > $@

-include deps

$(OBJSDIR)%.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBFILE): $(OBJS)
	$(AR) $(ARFLAGS) $(LIBFILE) $(OBJS)

clean: 
	rm -f $(OBJSDIR)/*.o
	rm -f $(LIBFILE)
	rm -f deps


.PHONY: clean install all
