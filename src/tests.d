# Note: 99+ required for-loop initial declaration (CentOS 6)

CFLAGS   += -Wall -pedantic -I./ -I ../ -std=c99
CXXFLAGS += -Wall -pedantic -I./ -I ../
LDFLAGS  += -lcheck

ifdef DEBUG
CFLAGS   += -ggdb -O0
CXXFLAGS += -ggdb -O0
endif

.PHONY: all clean mrpropre mrpropre test tests run run-more

all: test

tests: test

test: tests/launcher.o
	$(MAKE) -C tests/ -f utils.d

clean:
	$(MAKE) -C tests/ -f utils.d clean
	rm -f tests/launcher.o 

mrproper: mrpropre

mrpropre: clean
	$(MAKE) -C tests/ -f utils.d mrpropre

run:
	$(MAKE) -C tests/ -f utils.d run

run-more:
	$(MAKE) -C tests/ -f utils.d run-more
