DEBUG=yes
ifeq ($(DEBUG),yes)
	DEBUG_CFLAGS=-g
else
	DEBUG_CFLAGS=-O2 -Os
endif
FORCE_RPATH=
ifeq ($(FORCE_RPATH),)
	FORCE_RPATH_STR=
else
	FORCE_RPATH_STR=-Wl,-rpath=$(FORCE_RPATH)
endif
PREFIX=/usr/local
CFLAGS+=-D_XOPEN_SOURCE=700

_LDFLAGS=$(LDFLAGS) -L. $(shell pkg-config --libs glib-2.0) $(FORCE_RPATH_STR)
_CFLAGS=$(CFLAGS) -I. $(shell pkg-config --cflags glib-2.0) -fPIC -Wall -std=c99 -Wextra -pedantic -Werror -Wshadow -Wstrict-overflow -fno-strict-aliasing -DG_LOG_DOMAIN=\"log_proxy\" $(DEBUG_CFLAGS)

CC=gcc

OBJECTS=util.o control.o out.o
BINARIES=log_proxy
LIBS=

all: $(OBJECTS) $(BINARIES) $(LIBS)

clean:
	rm -f $(OBJECTS) $(BINARIES) core.* vgcore.*

log_proxy: log_proxy.c $(OBJECTS) options.h
	$(CC) $(_CFLAGS) $(_LDFLAGS) -o $@ $^

control.o: control.c control.h
	$(CC) -c -o $@ $(_CFLAGS) $<

out.o: out.c out.h
	$(CC) -c -o $@ $(_CFLAGS) $<

util.o: util.c util.h
	$(CC) -c -o $@ $(_CFLAGS) $<

install:
	mkdir -p $(PREFIX)/bin
	cp -f $(BINARIES) $(PREFIX)/bin/
