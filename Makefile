# Simplified Makefile for femon_json

binaries = femon_json

CPPFLAGS += -I../../lib
LDFLAGS  += -L../../lib/libdvbapi
LDLIBS   += -ldvbapi -ljson-c -lm

.PHONY: all clean install

all: $(binaries)

femon_json: femon_json.o
	$(CC) $(CPPFLAGS) -o $@ femon_json.o $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) $(binaries) femon_json.o

install: all
	install -d $(DESTDIR)/usr/bin
	install -m 0755 $(binaries) $(DESTDIR)/usr/bin

