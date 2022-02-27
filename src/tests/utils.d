# Note: 99+ required for-loop initial declaration (CentOS 6)

CFLAGS   += -Wall -pedantic -I./ -I ../ -std=c99
CXXFLAGS += -Wall -pedantic -I./ -I ../
LDFLAGS  += -lcheck
ifdef DEBIAN
LDFLAGS  += -lsubunit -lm -lpthread -lrt
endif

ifdef DEBUG
CFLAGS   += -ggdb -O0
CXXFLAGS += -ggdb -O0
endif

.PHONY: all clean mrpropre mrpropre test tests run run-more

SOURCES=$(wildcard utils/*.c)
OBJECTS=$(SOURCES:%.c=%.o)

all: test

tests: test

test: launcher.o ../../bin/libutils.o $(OBJECTS)
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/utils $(LDFLAGS)

run:
	bin/utils

run-more:
	bin/utils --more

clean:
	rm -f utils/*.o

mrproper: mrpropre

mrpropre: clean
	rm -f bin/utils
	rmdir bin || true
