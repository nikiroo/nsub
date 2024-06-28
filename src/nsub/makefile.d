# Note: 99+ required for for-loop initial declaration (CentOS 6)
# Note: gnu99 (instead of c99) required for libcutils-net

NAME   = nsub
NAMES  = $(NAME)
srcdir = $(NAME)
dstdir = ../bin

CFLAGS   += -Wall -pedantic -I./ -std=gnu99
CXXFLAGS += -Wall -pedantic -I./
# LDFLAGS  += -lcheck
PREFIX   =  /usr/local

ifdef DEBUG
CFLAGS   += -ggdb -O0
CXXFLAGS += -ggdb -O0
endif

.PHONY: all build install uninstall clean mrpropre mrpropre \
	$(NAMES)

SOURCES=$(wildcard $(srcdir)/*.c)
HEADERS=$(wildcard $(srcdir)/*.h)
OBJECTS=$(SOURCES:%.c=%.o)

# Main targets

all: build

build: $(NAMES)

nsub: $(dstdir)/nsub

run:
	$(dstdir)/nsub --help

################
# Dependencies #
################
OBJECTS+=$(dstdir)/libcutils.o
$(dstdir)/libcutils.o:
	$(MAKE) --no-print-directory -C cutils/ cutils
################

$(dstdir)/nsub: $(OBJECTS)
	mkdir -p $(dstdir)
	$(CC) $(OBJECTS) -o $@

debug: 
	$(MAKE) -f $(srcdir)/makefile.d DEBUG=1

clean:
	rm -f $(srcdir)/*.o

mrproper: mrpropre

mrpropre: clean
	rm -f $(dstdir)/nsub
	rmdir $(dstdir) 2>/dev/null || true

install: 
	mkdir -p "$(PREFIX)/bin"
	cp $(dstdir)/nsub "$(PREFIX)/bin/"

uninstall:
	rm "$(PREFIX)/bin/nsub"
	rmdir "$(PREFIX)/bin" 2>/dev/null

