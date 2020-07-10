.PHONY: all clean install test leak coverage release

all:
	cd src && $(MAKE) all

clean:
	cd src && $(MAKE) clean
	rm -Rf release

install:
	cd src && $(MAKE) install

test:
	cd src && $(MAKE) test

leak:
	cd src && $(MAKE) leak

coverage:
	cd src && $(MAKE) coverage

release: clean
	cd src && $(MAKE) STATIC=1 DESTDIR=$(shell pwd)/release install
