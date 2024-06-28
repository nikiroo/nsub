#
# Simple makefile that will forward all commands to src/xxx
# > NAME: main program (for 'man' and 'run')
# > NAMES: list of all the programs to compile
# > TESTS: list of all test programs to compile and run
#
NAME   = nsub
NAMES  = $(NAME) cutils
TESTS  = 

PREFIX = /usr/local
dstdir = bin

.PHONY: all build run clean mrpropre mrpropre love debug doc man \
	test run-test run-test-more \
	mess-build mess-run mess-clean mess-propre mess-doc mess-man \
	mess-test mess-run-test mess-run-test-more \
	$(NAMES) $(TESTS)

all: build

build: mess-build $(NAMES)

test: mess-test $(TESTS)

# Main buildables
M_OPTS=$(MAKECMDGOALS) --no-print-directory PREFIX=$(PREFIX) DEBUG=$(DEBUG)
$(NAMES) $(TESTS):
	$(MAKE) -C src/$@ $(M_OPTS) 

# Manual
man: mess-man
	@$(MAKE) -f man.d $(MAKECMDGOALS) NAME=$(NAME)

# Run
run: mess-run $(NAME)

# Run main test
run-test: mess-run-test $(TESTS)
run-test-more: mess-run-test-more $(TESTS)

# Doc/man/misc
doc: mess-doc
	doxygen
love:
	@echo " ...not war."
debug:
	$(MAKE) $(MAKECMDGOALS) PREFIX=$(PREFIX) NAME=$(NAME) DEBUG=1

# Clean
clean: mess-clean $(TESTS) $(NAMES)
mrproper: mrpropre
mrpropre: mess-propre $(TESTS) $(NAMES) man
	rm -rf doc/html doc/latex doc/man
	rmdir doc 2>/dev/null || true

# Install/uninstall
install: mess-install $(NAMES) man
uninstall: mess-uninstall $(NAMES) man

# Messages
mess-build:
	@echo
	@echo ">>>>>>>>>> Building $(NAMES)..."
mess-run:
	@echo
	@echo ">>>>>>>>>> Running $(NAME)..."
mess-clean:
	@echo
	@echo ">>>>>>>>>> Cleaning $(NAMES) $(TESTS)..."
mess-propre:
	@echo
	@echo ">>>>>>>>>> Calling Mr Propre..."
mess-doc:
	@echo
	@echo ">>>>>>>>>> Generating documentation for $(NAME)..."
mess-man:
	@echo
	@echo ">>>>>>>>>> Manual of $(NAME): $(MAKECMDGOALS)..."
mess-test:
	@echo
	@echo ">>>>>>>>>> Building all tests: $(TESTS)..."
mess-run-test:
	@echo
	@echo ">>>>>>>>>> Running tests: $(TESTS)..."
mess-run-test-more:
	@echo
	@echo ">>>>>>>>>> Running more tests: $(TESTS)..."
mess-install:
	@echo
	@echo ">>>>>>>>>> Installing $(NAME) into $(PREFIX)..."
mess-uninstall:
	@echo
	@echo ">>>>>>>>>> Uninstalling $(NAME) from $(PREFIX)..."

