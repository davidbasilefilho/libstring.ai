CC = gcc
# Default to linux/amd64 target
TARGET_OS ?= linux
TARGET_ARCH ?= amd64
BUILD_TYPE ?= debug

# Compiler settings
CFLAGS_COMMON = -std=c2x -Wall -Wextra -pedantic -fPIC
CFLAGS_DEBUG = $(CFLAGS_COMMON) -g -O0 -DDEBUG
# Enhanced optimization flags for release builds
CFLAGS_RELEASE = $(CFLAGS_COMMON) \
    -O3
\
    -march=native \
    -mtune=native \
    -flto \
    -floop-optimize \
    -funroll-loops \
    -finline-functions \
    -ftree-vectorize \
    -fomit-frame-pointer \
    -DNDEBUG

# Set flags based on build type
ifeq ($(BUILD_TYPE),debug)
    CFLAGS = $(CFLAGS_DEBUG)
    BUILD_SUFFIX = debug
else
    CFLAGS = $(CFLAGS_RELEASE)
    BUILD_SUFFIX = release
endif

# Linker settings
LDFLAGS_COMMON = -shared
LDFLAGS = $(LDFLAGS_COMMON)

# Target-specific settings
ifeq ($(TARGET_OS),windows)
    BINEXT = .exe
    LIBEXT = .dll
    ifeq ($(TARGET_ARCH),amd64)
        CC = x86_64-w64-mingw32-gcc
    else ifeq ($(TARGET_ARCH),386)
        CC = i686-w64-mingw32-gcc
    else ifeq ($(TARGET_ARCH),arm64)
        CC = aarch64-w64-mingw32-gcc
    endif
else ifeq ($(TARGET_OS),darwin)
    BINEXT = 
    LIBEXT = .dylib
    ifeq ($(TARGET_ARCH),amd64)
        CC = o64-clang
    else ifeq ($(TARGET_ARCH),arm64)
        CC = oa64-clang
    endif
    LDFLAGS += -install_name @rpath/libstring$(LIBEXT)
else # linux
    BINEXT = 
    LIBEXT = .so
    ifeq ($(TARGET_ARCH),arm64)
        CC = aarch64-linux-gnu-gcc
    endif
endif

# Output directory with target-specific subdirectories
BINDIR = bin/$(TARGET_OS)_$(TARGET_ARCH)/$(BUILD_SUFFIX)

# Library source files
LIB_SRC = string_lib.c
LIB_OBJ = $(BINDIR)/string_lib.o
LIB_NAME_BASE = libstring
LIB_NAME = $(BINDIR)/$(LIB_NAME_BASE)$(LIBEXT)
STATIC_LIB_NAME = $(BINDIR)/$(LIB_NAME_BASE).a

# Test program
TEST_SRC = test_string.c
TEST_BIN = $(BINDIR)/test_string$(BINEXT)

.PHONY: all clean test static shared debug release list-targets all-targets check-compilers

# Define compiler check functions
check-compiler = which $(1) > /dev/null 2>&1
check-linux-amd64 = $(call check-compiler,gcc)
check-linux-arm64 = $(call check-compiler,aarch64-linux-gnu-gcc)
check-windows-amd64 = $(call check-compiler,x86_64-w64-mingw32-gcc)
check-windows-arm64 = $(call check-compiler,aarch64-w64-mingw32-gcc)
check-darwin-amd64 = $(call check-compiler,o64-clang)
check-darwin-arm64 = $(call check-compiler,oa64-clang)

# Check available compilers and print status
check-compilers:
	@echo "Checking available compilers:"
	@if $(call check-linux-amd64); then echo "  ✓ Linux AMD64 (gcc)"; else echo "  ✗ Linux AMD64 (gcc not found)"; fi
	@if $(call check-linux-arm64); then echo "  ✓ Linux ARM64 (aarch64-linux-gnu-gcc)"; else echo "  ✗ Linux ARM64 (aarch64-linux-gnu-gcc not found)"; fi
	@if $(call check-windows-amd64); then echo "  ✓ Windows AMD64 (x86_64-w64-mingw32-gcc)"; else echo "  ✗ Windows AMD64 (x86_64-w64-mingw32-gcc not found)"; fi
	@if $(call check-windows-arm64); then echo "  ✓ Windows ARM64 (aarch64-w64-mingw32-gcc)"; else echo "  ✗ Windows ARM64 (aarch64-w64-mingw32-gcc not found)"; fi
	@if $(call check-darwin-amd64); then echo "  ✓ macOS AMD64 (o64-clang)"; else echo "  ✗ macOS AMD64 (o64-clang not found)"; fi
	@if $(call check-darwin-arm64); then echo "  ✓ macOS ARM64 (oa64-clang)"; else echo "  ✗ macOS ARM64 (oa64-clang not found)"; fi

# Build all targets with available compilers
all-targets:
	@echo "Building targets with available compilers..."
	@if $(call check-linux-amd64); then $(MAKE) linux-amd64-all; else echo "Skipping Linux AMD64 (compiler not found)"; fi
	@if $(call check-linux-arm64); then $(MAKE) linux-arm64-all; else echo "Skipping Linux ARM64 (compiler not found)"; fi
	@if $(call check-windows-amd64); then $(MAKE) windows-amd64-all; else echo "Skipping Windows AMD64 (compiler not found)"; fi
	@if $(call check-windows-arm64); then $(MAKE) windows-arm64-all; else echo "Skipping Windows ARM64 (compiler not found)"; fi
	@if $(call check-darwin-amd64); then $(MAKE) darwin-amd64-all; else echo "Skipping macOS AMD64 (compiler not found)"; fi
	@if $(call check-darwin-arm64); then $(MAKE) darwin-arm64-all; else echo "Skipping macOS ARM64 (compiler not found)"; fi

# Default target builds current platform
all: shared static test

# Target type shortcuts
debug:
	$(MAKE) BUILD_TYPE=debug

release:
	$(MAKE) BUILD_TYPE=release

# Build for all major targets (add or remove as needed)
linux-amd64-all: linux-amd64-debug linux-amd64-release

linux-amd64-debug:
	$(MAKE) shared static test TARGET_OS=linux TARGET_ARCH=amd64 BUILD_TYPE=debug

linux-amd64-release:
	$(MAKE) shared static test TARGET_OS=linux TARGET_ARCH=amd64 BUILD_TYPE=release

linux-arm64-all: linux-arm64-debug linux-arm64-release

linux-arm64-debug:
	$(MAKE) shared static test TARGET_OS=linux TARGET_ARCH=arm64 BUILD_TYPE=debug

linux-arm64-release:
	$(MAKE) shared static test TARGET_OS=linux TARGET_ARCH=arm64 BUILD_TYPE=release

windows-amd64-all: windows-amd64-debug windows-amd64-release

windows-amd64-debug:
	$(MAKE) shared static test TARGET_OS=windows TARGET_ARCH=amd64 BUILD_TYPE=debug

windows-amd64-release:
	$(MAKE) shared static test TARGET_OS=windows TARGET_ARCH=amd64 BUILD_TYPE=release

windows-arm64-all: windows-arm64-debug windows-arm64-release

windows-arm64-debug:
	$(MAKE) shared static test TARGET_OS=windows TARGET_ARCH=arm64 BUILD_TYPE=debug

windows-arm64-release:
	$(MAKE) shared static test TARGET_OS=windows TARGET_ARCH=arm64 BUILD_TYPE=release

darwin-arm64-all: darwin-arm64-debug darwin-arm64-release

darwin-arm64-debug:
	$(MAKE) shared static test TARGET_OS=darwin TARGET_ARCH=arm64 BUILD_TYPE=debug

darwin-arm64-release:
	$(MAKE) shared static test TARGET_OS=darwin TARGET_ARCH=arm64 BUILD_TYPE=release

darwin-amd64-all: darwin-amd64-debug darwin-amd64-release

darwin-amd64-debug:
	$(MAKE) shared static test TARGET_OS=darwin TARGET_ARCH=amd64 BUILD_TYPE=debug

darwin-amd64-release:
	$(MAKE) shared static test TARGET_OS=darwin TARGET_ARCH=amd64 BUILD_TYPE=release

# Create output directory
$(BINDIR):
	mkdir -p $(BINDIR)

# Compile shared library
shared: $(LIB_NAME)

$(LIB_NAME): $(LIB_OBJ) | $(BINDIR)
	$(CC) $(LDFLAGS) -o $@ $^

# Compile static library
static: $(STATIC_LIB_NAME)

$(STATIC_LIB_NAME): $(LIB_OBJ) | $(BINDIR)
	ar rcs $@ $^

# Compile test program
test: $(TEST_BIN)

$(TEST_BIN): $(TEST_SRC) $(LIB_NAME) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $< -L$(BINDIR) -lstring -Wl,-rpath,'$$ORIGIN'

# Compile source files to object files
$(BINDIR)/%.o: %.c | $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf bin

clean-target:
	rm -rf $(BINDIR)

# Display available targets
list-targets:
	@echo "Available targets:"
	@echo "  make                    - Build for current OS/arch in debug mode"
	@echo "  make debug              - Build debug version"
	@echo "  make release            - Build release version"
	@echo "  make linux-amd64        - Build for Linux (AMD64) in both debug and release modes"
	@echo "  make linux-arm64        - Build for Linux (ARM64) in both debug and release modes"
	@echo "  make windows-amd64      - Build for Windows (AMD64) in both debug and release modes"
	@echo "  make windows-arm64      - Build for Windows (ARM64) in both debug and release modes"
	@echo "  make darwin-arm64       - Build for macOS (ARM64) in both debug and release modes"
	@echo "  make darwin-amd64       - Build for macOS (AMD64) in both debug and release modes"
	@echo "  make all-targets        - Build for all supported platforms"
	@echo ""
	@echo "You can also specify individual target-mode combinations:"
	@echo "  make linux-amd64-debug, make windows-amd64-release, etc."
	@echo ""
	@echo "Or set variables manually:"
	@echo "  make TARGET_OS=linux TARGET_ARCH=amd64 BUILD_TYPE=release"

# Install library (can be run with sudo)
install: $(LIB_NAME)
	install -d $(DESTDIR)/usr/local/include/
	install -d $(DESTDIR)/usr/local/lib/
	install -m 644 string_lib.h $(DESTDIR)/usr/local/include/
	install -m 755 $(LIB_NAME) $(DESTDIR)/usr/local/lib/
	ldconfig