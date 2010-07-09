CC=gcc
NAMS=nams
CFLAGS=-g -O0
LDFLAGS=-g
OBJS=$(SOURCES:.c=.o)
SOURCES=$(wildcard *.c)
HEADERS=$(wildcard *.h)
PROGNAME=kernel.bin

$(PROGNAME): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

deps: $(SOURCES)
	$(CC) $(CFLAGS) -MM $(SOURCES) > $@  
-include deps

clean:
	rm -f ./*.o
	rm -f $(PROGNAME)

new: clean $(PROGNAME)

entrega: $(SOURCES) $(HEADERS) Makefile
	tar zcvf entrega.tar.gz $(SOURCES) $(HEADERS) Makefile

