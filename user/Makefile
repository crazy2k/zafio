INCLUDE := ../inc/
LIBS := ../lib/

CC := gcc
CFLAGS := -std=gnu99 -Wall -nostdlib -nostdinc -nostartfiles -nodefaultlibs -I$(INCLUDE)

LD := ld
LDFLAGS := --whole-archive -L$(LIBS) -lzafiostd 

PROGS_DIR := ../../progs/
BINARY := $(shell basename `pwd`)

ENTRY := main

SOURCES := $(wildcard *.c)
HEADERS := $(wildcard *.h)

OBJS := $(SOURCES:.c=.o)
PROG_FILE := $(PROGS_DIR)/$(BINARY)

all: $(BINARY)

deps: $(SOURCES)
	$(CC) $(CFLAGS) -MM $(SOURCES) > $@

-include deps

%.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(BINARY): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@ --entry=$(ENTRY)
        

install: $(BINARY)
	mkdir -p $(PROGS_DIR)
	mv $(BINARY) $(PROGS_DIR)

clean: 
	rm -f *.o
	rm -f $(BINARY)
	rm -f $(PROG_FILE)
	rm -f deps


.PHONY: clean install all
