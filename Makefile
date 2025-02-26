CC = gcc
CFLAGS = -std=c2x -Wall -Wextra -pedantic -g -fPIC
LDFLAGS = -shared

# Output directory
BINDIR = bin

# Library source files
LIB_SRC = string_lib.c
LIB_OBJ = $(BINDIR)/$(LIB_SRC:.c=.o)
LIB_NAME = $(BINDIR)/libstring.so

# Test program
TEST_SRC = test_string.c
TEST_BIN = $(BINDIR)/test_string

.PHONY: all clean test static shared

all: $(BINDIR) shared static test

$(BINDIR):
	mkdir -p $(BINDIR)

# Compile shared library
shared: $(LIB_NAME)

$(LIB_NAME): $(LIB_OBJ) | $(BINDIR)
	$(CC) $(LDFLAGS) -o $@ $^

# Compile static library
static: $(BINDIR)/libstring.a

$(BINDIR)/libstring.a: $(LIB_OBJ) | $(BINDIR)
	ar rcs $@ $^

# Compile test program
test: $(TEST_BIN)

$(TEST_BIN): $(TEST_SRC) $(LIB_NAME) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $< -L$(BINDIR) -lstring -Wl,-rpath,'$$ORIGIN'

# Compile source files to object files
$(BINDIR)/%.o: %.c | $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BINDIR)

# Install library (can be run with sudo)
install: $(LIB_NAME)
	install -m 644 string_lib.h /usr/local/include/
	install -m 755 $(LIB_NAME) /usr/local/lib/
	ldconfig