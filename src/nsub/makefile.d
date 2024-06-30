# Note: 99+ required for for-loop initial declaration (CentOS 6)

NAME   = nsub
NAMES  = $(NAME)
srcdir = $(NAME)

ifeq ($(dstdir),)
dstdir = $(srcdir)/bin
endif

CFLAGS   += -Wall -pedantic -I./ -std=c99
CXXFLAGS += -Wall -pedantic -I./
# LDFLAGS  += -lcheck
PREFIX   =  /usr/local

ifdef DEBUG
CFLAGS   += -ggdb -O0
CXXFLAGS += -ggdb -O0
endif

.PHONY: all build install uninstall clean mrpropre mrpropre \
	$(NAMES) deps test run run-test run-test-more

SOURCES=$(wildcard $(srcdir)/*.c)
OBJECTS=$(SOURCES:%.c=%.o)

# Main targets

all: build

build: $(NAMES)

$(NAME): deps $(dstdir)/$(NAME)

# Program, so no test
run:
	@echo
	$(dstdir)/$(NAME) --help
test run-test run-test-more:
	@echo you are in the sources of the program, look at the tests instead

################
# Dependencies #
################
## Libs (if needed)
deps:
	$(MAKE) --no-print-directory -C cutils/ cutils
DEPS=$(dstdir)/libcutils.o

## Headers
DEPENDS=$(SOURCES:%.c=%.d)
-include $(DEPENDS)
%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@
################

$(dstdir)/$(NAME): $(DEPS) $(OBJECTS)
	mkdir -p $(dstdir)
	# note: -r = --relocatable, but former also works with Clang
	$(CC) $(CFLAGS) $(LDFLAGS) $(DEPS) $(OBJECTS) -o $@

debug: 
	$(MAKE) -f $(srcdir)/makefile.d DEBUG=1

clean:
	rm -f $(OBJECTS)
	rm -f $(DEPENDS)
	rm -f $(DEPS)

mrproper: mrpropre
mrpropre: clean
	rm -f $(dstdir)/$(NAME)
	rmdir $(dstdir) 2>/dev/null || true

install: build
	mkdir -p "$(PREFIX)/bin"
	cp "$(dstdir)/$(NAME)" "$(PREFIX)/bin/"

uninstall: build
	rm "$(PREFIX)/bin/$(NAME)"
	rmdir "$(PREFIX)/bin" 2>/dev/null

