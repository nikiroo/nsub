.PHONY: all doc install uninstall clean mrpropre mrpropre love debug

all: bin/nsub

doc:
	doxygen

bin/nsub: bin/utils.o
	$(MAKE) -C src -f nsub.d

bin/utils.o:
	$(MAKE) -C src -f utils.d

love:
	@echo " ...not war."

debug:
	$(MAKE) DEBUG=1

clean:
	$(MAKE) -C src -f utils.d clean
	$(MAKE) -C src -f nsub.d  clean

mrproper: mrpropre

mrpropre: clean
	$(MAKE) -C src -f utils.d mrpropre
	$(MAKE) -C src -f nsub.d  mrpropre

install:
	$(MAKE) -C src -f nsub.d install

uninstall:
	$(MAKE) -C src -f nsub.d uninstall

