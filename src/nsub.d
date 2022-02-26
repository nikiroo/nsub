CFLAGS   += -Wall -I./
CXXFLAGS += -Wall -I./
PREFIX   =  /usr/local

ifdef DEBUG
CFLAGS   += -ggdb -O0
CXXFLAGS += -ggdb -O0
endif

.PHONY: all install uninstall clean mrpropre mrpropre debug

all: ../bin/nsub

SOURCES=$(wildcard nsub/*.c)
HEADERS=$(wildcard nsub/*.h)

## Correct, but longer:

## OBJECTS=$(SOURCES:%.c=%.o)
##
## nsub/nsub.o: nsub/nsub.c
##
## $(OBJECTS): nsub/nsub.h
## 
## # easier, but more recompiles:
## $(OBJECTS): $(HEADERS)
##
##../bin/nsub: $(OBJECTS) ../bin/libutils.o
##	$(CC) $(CFLAGS) $^ -o $@

## Quicker:
../bin/nsub: $(SOURCES) $(HEADERS) ../bin/libutils.o
	$(CC) $(CFLAGS) $^ -o $@

debug:
	$(MAKE) -f nsub.d DEBUG=1

clean:
	rm -f nsub/*.o

mrproper: mrpropre

mrpropre: clean
	rm -f ../bin/nsub

install: 
	@echo "installing nsub to $(PREFIX)..."
	mkdir -p "$(PREFIX)/bin" \
	&& cp ../bin/nsub "$(PREFIX)/bin/" \

uninstall:
	@echo "uninstalling nsub from $(PREFIX)..."
	rm -f "$(PREFIX)/bin/nsub" \
	&& rmdir "$(PREFIX)/bin" 2>/dev/null || true \
	&& rmdir "$(PREFIX)" 2>/dev/null || true

