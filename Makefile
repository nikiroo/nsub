NAME=nsub

.PHONY: all build rebuild install uninstall clean mrpropre mrpropre love debug \
	doc man run tests test run-test run-test-more

all: build

build: bin/nsub
	@echo Build successful.

run: bin/nsub
	bin/nsub
	
tests: test

test:
	$(MAKE) -C src -f tests.d
	
run-test:
	$(MAKE) -C src -f tests.d run

run-test-more:
	$(MAKE) -C src -f tests.d run-more

rebuild: clean build

doc:
	doxygen

man: VERSION README*.md
	$(MAKE) -f man.d NAME=$(NAME)

love:
	@echo " ...not war."

debug:
	$(MAKE) DEBUG=1

mrproper: mrpropre

mrpropre: clean
	$(MAKE) -C src -f tests.d mrpropre
	$(MAKE) -C src -f utils.d mrpropre
	$(MAKE) -C src -f nsub.d  mrpropre
	rm -f man/man1/*.1 man/*/man1/*.1
	rmdir man/*/man1 man/* man || true 
	rm -rf doc/html doc/latex doc/man
	rmdir doc || true

clean:
	$(MAKE) -C src -f tests.d clean
	$(MAKE) -C src -f utils.d clean
	$(MAKE) -C src -f nsub.d  clean

install:
	$(MAKE) -C src -f nsub.d install
	@if [ -e "man/man1/$(NAME).1" ]; then \
		cp -r man/ "$(PREFIX)"/share/; \
	else \
		echo "No manual has been built (see \`make man')"; \
	fi

uninstall:
	$(MAKE) -C src -f nsub.d uninstall

bin/nsub: bin/utils.o
	$(MAKE) -C src -f nsub.d DEBUG=$(DEBUG)

bin/utils.o:
	$(MAKE) -C src -f utils.d DEBUG=$(DEBUG)

