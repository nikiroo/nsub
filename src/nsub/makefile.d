#
# Makefile for C projets
# > NAME   : the name of the main program (if programs, make a single .d file 
#            per program, link them up in Makfile and use a $ssrcdir)
# > srcdir : the source directory
# > ssrcdir: the sub-sources directory (defaults to $srcdir)
# > dstdir: the destination directory (defaults to $srcdir/bin)
#
# Environment variables:
# > PREFIX: where to (un)install (defaults to /usr/local)
# > DEBUG: define it to build with all debug symbols
#
NAME    = nsub
srcdir  = $(NAME)
ssrcdir = $(srcdir)

# Note: c99+ required for for-loop initial declaration (not default in CentOS 6)
# Note: gnu99 can be required for some projects (i.e.: libcutils-net)
CFLAGS   += -Wall -pedantic -I./ -std=c99
CXXFLAGS += -Wall -pedantic -I./
PREFIX    =  /usr/local

# Required libraries if any:
# LDFLAGS += -lcheck

# Required *locally compiled* libraries if any:
LIBS       = cutils

################################################################################

ifeq ($(dstdir),)
dstdir = $(srcdir)/bin
endif

ifdef DEBUG
CFLAGS   += -ggdb -O0
CXXFLAGS += -ggdb -O0
endif

# Default target
.PHONY: all deps
all:

# locally compiled libs:
ifneq ($(LIBS),)
LDFLAGS   += -L$(dstdir)
LDFLAGS   += $(foreach lib,$(LIBS),-l$(lib))
endif
deps:
	$(foreach lib,$(LIBS),$(MAKE) --no-print-directory \
			-C $(lib)/ $(lib) dstdir=$(dstdir))

.PHONY: build rebuild install uninstall clean mrpropre mrpropre \
	$(NAME) test run run-test run-test-more

SOURCES=$(wildcard $(ssrcdir)/*.c)
OBJECTS=$(SOURCES:%.c=%.o)
DEPENDS =$(SOURCES:%.c=%.d)

# Autogenerate dependencies from code
-include $(DEPENDS)
%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Main targets

all: build

build: $(NAME)

rebuild: clean build

$(NAME): deps $(dstdir)/$(NAME)

# Program, so no test
run:
	@echo
	$(dstdir)/$(NAME) --help
test run-test run-test-more:
	@echo you are in the sources of the program, look at the tests instead

$(dstdir)/$(NAME): $(OBJECTS)
	mkdir -p $(dstdir)
	# note: LDFLAGS *must* be near the end
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

clean:
	$(foreach lib,$(LIBS),$(MAKE) --no-print-directory \
			-C $(lib)/ $@ dstdir=$(dstdir))
	rm -f $(OBJECTS)
	rm -f $(DEPENDS)

mrproper: mrpropre
mrpropre: clean
	$(foreach lib,$(LIBS),$(MAKE) --no-print-directory \
			-C $(lib)/ $@ dstdir=$(dstdir))
	rm -f $(dstdir)/$(NAME)
	rmdir $(dstdir) 2>/dev/null || true

install: build
	mkdir -p "$(PREFIX)/bin"
	cp "$(dstdir)/$(NAME)" "$(PREFIX)/bin/"

uninstall:
	rm "$(PREFIX)/bin/$(NAME)"
	rmdir "$(PREFIX)/bin" 2>/dev/null

