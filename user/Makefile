INCLUDE := ../stdlib/inc/
LIBDIR := ../lib/
STDLIBDIR := ../stdlib/

LIBNAME := zafiostd
LIBFILE := $(LIBDIR)/lib$(LIBNAME).a

CC := gcc
CFLAGS := -std=gnu99 -Wall -nostdlib -nostdinc -nostartfiles -nodefaultlibs -I$(INCLUDE)

LD := ld
LDFLAGS := --whole-archive -L$(LIBDIR) -lzafiostd

PROGS_DIR := ../../progs/
BINARY := $(shell basename `pwd`)

ENTRY := main

SOURCES := $(wildcard *.c)
HEADERS := $(wildcard *.h)

OBJS := $(SOURCES:.c=.o)
PROG_FILE := $(PROGS_DIR)/$(BINARY)

all: $(BINARY)

$(LIBDIR):
	mkdir $(LIBDIR)

deps: $(SOURCES)
	$(CC) $(CFLAGS) -MM $(SOURCES) > $@

-include deps

%.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBFILE): build_user_libs

$(BINARY): $(OBJS) $(LIBFILE)
	$(LD) $(LDFLAGS) $(OBJS) -o $@ --entry=$(ENTRY)
        
build_user_libs: $(LIBDIR)
	$(MAKE) -C $(STDLIBDIR) install

install: $(BINARY)
	mkdir -p $(PROGS_DIR)
	mv $(BINARY) $(PROGS_DIR)

clean: 
	rm -f *.o
	rm -f $(BINARY)
	rm -f $(PROG_FILE)
	rm -f deps


.PHONY: clean install all build_user_libs
