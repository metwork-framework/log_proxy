.PHONY: coverage leak test clean all

DEBUG=no
FORCE_RPATH=
DESTDIR=
PREFIX=/usr/local
STATIC=no
GCC?=gcc
CFLAGS+=-D_XOPEN_SOURCE=700
GCC_VERSION=`$(GCC) --version | head -1 | cut -d" " -f3 | cut -d"." -f1-3`

ifeq ($(COVERAGE),yes)
    COVERAGE_CFLAGS=-fprofile-arcs -ftest-coverage
else
    COVERAGE_CFLAGS=
endif
ifeq ($(STATIC),yes)
    _STATIC_EXTRA_OPT=--static
    _STATIC_OPT=-static
else
    _STATIC_EXTRA_OPT=
    _STATIC_OPT=
endif
ifeq ($(FORCE_RPATH),)
	FORCE_RPATH_STR=
else
	FORCE_RPATH_STR=-Wl,-rpath=~$(FORCE_RPATH)~
endif
ifeq ($(DEBUG),yes)
	DEBUG_CFLAGS=-g -Werror
else
	DEBUG_CFLAGS=-O2 -Os
endif
ifeq ($(shell expr $(GCC_VERSION) \< "8.0.0" ), 1)
    _CFLAGS=$(CFLAGS) -I. $(shell pkg-config --cflags $(_STATIC_EXTRA_OPT) glib-2.0) -fPIC -Wall -std=c99 -Wextra -pedantic -Wshadow -Wstrict-overflow -Wno-deprecated-declarations -fno-strict-aliasing -DG_LOG_DOMAIN=\"log_proxy\" $(DEBUG_CFLAGS) $(COVERAGE_CFLAGS)
else
    _CFLAGS=$(CFLAGS) -I. $(shell pkg-config --cflags $(_STATIC_EXTRA_OPT) glib-2.0) -fPIC -Wall -std=c99 -Wextra -pedantic -Wshadow -Wstrict-overflow -Wno-deprecated-declarations -Wno-cast-function-type -fno-strict-aliasing -DG_LOG_DOMAIN=\"log_proxy\" $(DEBUG_CFLAGS) $(COVERAGE_CFLAGS)
endif
_LDFLAGS=$(LDFLAGS) -L. $(shell pkg-config --libs $(_STATIC_EXTRA_OPT) glib-2.0) $(shell echo '$(FORCE_RPATH_STR)' |sed 's/@/$$/g' |sed s/~/"'"/g) -lrt

OBJECTS=util.o control.o out.o
BINARIES=log_proxy log_proxy_wrapper
TESTS=test_log_proxy
LIBS=

VALGRIND=./valgrind.sh

all:: $(OBJECTS) $(BINARIES) $(TESTS) $(LIBS)

clean::
	rm -f $(OBJECTS) $(BINARIES) $(TESTS) core.* vgcore.*
	rm -Rf coverage
	rm -f app.info *.gcno *.gcda

log_proxy: log_proxy.c $(OBJECTS) options.h
	$(GCC) $(_STATIC_OPT) $(_CFLAGS) -o $@ $^ $(_LDFLAGS)

log_proxy_wrapper: log_proxy_wrapper.c $(OBJECTS) options.h
	$(GCC) $(_STATIC_OPT) $(_CFLAGS) -o $@ $^ $(_LDFLAGS)

test_log_proxy: test_log_proxy.c $(OBJECTS)
	$(GCC) $(_STATIC_OPT) $(_CFLAGS) -o $@ $^ $(_LDFLAGS)

control.o: control.c control.h
	$(GCC) -c -o $@ $(_CFLAGS) $<

out.o: out.c out.h
	$(GCC) -c -o $@ $(_CFLAGS) $<

util.o: util.c util.h
	$(GCC) -c -o $@ $(_CFLAGS) $<

install: $(BINARIES)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(BINARIES) $(DESTDIR)$(PREFIX)/bin

test: $(TESTS)
	./test_log_proxy && echo "OK"

leak: $(TESTS)
	$(VALGRIND) ./test_log_proxy && echo "OK"

coverage:
	$(MAKE) clean
	$(MAKE) COVERAGE=yes test_log_proxy
	rm -Rf coverage/* app*.info && lcov --directory . --zerocounters
	./test_log_proxy
	lcov --directory . --capture --output-file app.info
	if ! test -d coverage; then mkdir coverage; fi; genhtml --output-directory coverage app.info
