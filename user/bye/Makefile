CC := gcc
CFLAGS := -std=gnu99 -Wall -nostdlib -nostdinc -nostartfiles -nodefaultlibs

LD := ld

PROGS_DIR := ../../progs/
BINARY := $(shell basename `pwd`)

ENTRY := main

SOURCES := $(wildcard *.c)
HEADERS := $(wildcard *.h)

OBJS := $(SOURCES:.c=.o)
PROG_FILE := $(PROGS_DIR)/$(BINARY)

deps: $(SOURCES)
	$(CC) $(CFLAGS) -MM $(SOURCES) > $@

-include deps

%.o:
	$(CC) $(CFLAGS) -c $< -o $@

all: $(BINARY)

$(BINARY): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@ --entry=$(ENTRY)

install: $(BINARY)
	cp $(BINARY) $(PROGS_DIR)

clean: 
	rm -f *.o
	rm -f $(BINARY)
	rm -f $(PROG_FILE)
	rm -f deps


.PHONY: clean install all
