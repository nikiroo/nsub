CFLAGS   += -Wall -I./ -std=c99
CXXFLAGS += -Wall -I./
PREFIX   =  /usr/local

ifdef DEBUG
CFLAGS   += -ggdb -O0
CXXFLAGS += -ggdb -O0
endif

.PHONY: all install uninstall clean mrpropre mrpropre

all: ../bin/libutils.o

SOURCES=$(wildcard utils/*.c)
HEADERS=$(wildcard utils/*.h)
OBJECTS=$(SOURCES:%.c=%.o)

utils/array.o:   utils/array.[ch]
utils/desktop.o: utils/desktop.[ch] utils/array.h

# easier, but more recompiles:
# $(OBJECTS): $(HEADERS)

../bin/libutils.o: $(OBJECTS)
	mkdir -p ../bin
	$(LD) --relocatable $(OBJECTS) -o $@

clean:
	rm -f utils/*.o

mrproper: mrpropre

mrpropre: clean
	rm -f ../bin/libutils.o 
	rmdir ../bin || true

install: 
	@echo "installing utils to $(PREFIX)..."
	mkdir -p "$(PREFIX)/lib/cutils/" \
	&& cp ../bin/libutils.o "$(PREFIX)/lib/cutils/" \
	&& mkdir -p "$(PREFIX)/include/cutils/utils/" \
	&& cp utils/*.h "$(PREFIX)/include/cutils/utils/" \

uninstall:
	@echo "uninstalling utils from $(PREFIX)..."
	rm -f "$(PREFIX)/lib/cutils/utils.o" \
	&& rmdir "$(PREFIX)/lib/cutils/utils" 2>/dev/null || true \
	&& rm -f "$(PREFIX)/include/cutils/utils/"*.h \
	&& rmdir "$(PREFIX)/include/cutils/utils" 2>/dev/null || true \
	&& rmdir "$(PREFIX)/lib/cutils" 2>/dev/null || true \
	&& rmdir "$(PREFIX)/include/cutils" 2>/dev/null || true

