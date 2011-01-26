INCLUDE := ./

CC := gcc
CFLAGS := -std=gnu99 -Wall -nostdlib -nostdinc -nostartfiles -nodefaultlibs -I$(INCLUDE)

AR := ar
ARFLAGS := -cvq

LIB_DIR := ../lib/
LIB_NAME := zafiostd
LIB_FILE := $(LIB_DIR)/lib$(LIB_NAME).a

SOURCES := $(wildcard *.c)
HEADERS := $(wildcard *.h)

OBJS := $(SOURCES:.c=.o)

all: $(LIB_FILE)

install: $(LIB_FILE)

deps: $(SOURCES)
	$(CC) $(CFLAGS) -MM $(SOURCES) > $@

-include deps

%.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_FILE): $(OBJS)
	$(AR) $(ARFLAGS) $(LIB_FILE) $(OBJS)

clean: 
	rm -f *.o
	rm -f $(LIB_FILE)
	rm -f deps


.PHONY: clean install all
