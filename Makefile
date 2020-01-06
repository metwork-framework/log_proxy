all:
	cd src && $(MAKE) all

clean:
	cd src && $(MAKE) clean

install:
	cd src && $(MAKE) install

test:
	cd src && $(MAKE) test

leak:
	cd src && $(MAKE) leak

coverage:
	cd src && $(MAKE) coverage
