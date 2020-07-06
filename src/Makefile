DEBUG=no
ifeq ($(DEBUG),yes)
	DEBUG_CFLAGS=-g
else
	DEBUG_CFLAGS=-O2 -Os
endif
FORCE_RPATH=
ifeq ($(FORCE_RPATH),)
	FORCE_RPATH_STR=
else
	FORCE_RPATH_STR=-Wl,-rpath=~$(FORCE_RPATH)~
endif

DESTDIR?=/usr/local/bin
GCC?=gcc

CFLAGS+=-D_XOPEN_SOURCE=700

ifeq ($(COVERAGE),yes)
    COVERAGE_CFLAGS=-fprofile-arcs -ftest-coverage
else
    COVERAGE_CFLAGS=
endif

GCC_VERSION=`$(GCC) --version | head -1 | cut -d" " -f3 | cut -d"." -f1-3`

ifeq ($(shell expr $(GCC_VERSION) \< "8.0.0" ), 1)
    _CFLAGS=$(CFLAGS) -I. $(shell pkg-config --cflags glib-2.0) -fPIC -Wall -std=c99 -Wextra -pedantic -Werror -Wshadow -Wstrict-overflow -Wno-deprecated-declarations -fno-strict-aliasing -DG_LOG_DOMAIN=\"log_proxy\" $(DEBUG_CFLAGS) $(COVERAGE_CFLAGS)
else
    _CFLAGS=$(CFLAGS) -I. $(shell pkg-config --cflags glib-2.0) -fPIC -Wall -std=c99 -Wextra -pedantic -Werror -Wshadow -Wstrict-overflow -Wno-deprecated-declarations -Wno-cast-function-type -fno-strict-aliasing -DG_LOG_DOMAIN=\"log_proxy\" $(DEBUG_CFLAGS) $(COVERAGE_CFLAGS)
endif

_LDFLAGS=$(LDFLAGS) -L. $(shell pkg-config --libs glib-2.0) $(shell echo '$(FORCE_RPATH_STR)' |sed 's/@/$$/g' |sed s/~/"'"/g)

.PHONY: coverage leak test clean all

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
	$(GCC) $(_CFLAGS) -o $@ $^ $(_LDFLAGS)

log_proxy_wrapper: log_proxy_wrapper.c $(OBJECTS) options.h
	$(GCC) $(_CFLAGS) -o $@ $^ $(_LDFLAGS)

test_log_proxy: test_log_proxy.c $(OBJECTS)
	$(GCC) $(_CFLAGS) -o $@ $^ $(_LDFLAGS)

control.o: control.c control.h
	$(GCC) -c -o $@ $(_CFLAGS) $<

out.o: out.c out.h
	$(GCC) -c -o $@ $(_CFLAGS) $<

util.o: util.c util.h
	$(GCC) -c -o $@ $(_CFLAGS) $<

install: $(BINARIES)
	mkdir -p $(DESTDIR)
	cp -f $(BINARIES) $(DESTDIR)

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
