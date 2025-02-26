# String Library for C

A simple and efficient string manipulation library for C, providing common string operations with a clean API. The library features Small String Optimization (SSO) and SIMD-accelerated operations where available.

**Note: This project was ENTIRELY created using AI models - specifically Claude 3.7 Sonnet (for the majority of the work) and Claude 3.5 Sonnet (used during rate limiting periods) from Anthropic, along with Visual Studio Code's Agent edit tool. No manual coding was required.**

## Project Overview

This project implements a modern, optimized string library for C that provides common string manipulation functions. The code is organized as follows:

- `string_lib.h` - Header file with function declarations and documentation
- `string_lib.c` - Implementation of the string functions
- `test_string.c` - Test program demonstrating the library's usage
- `Makefile` - Build configuration for compiling the library and tests

Build artifacts are stored in the `bin/` directory:
- `bin/libstring.so` - Shared library
- `bin/libstring.a` - Static library
- `bin/test_string` - Test executable

## Features

- Small String Optimization (SSO) for better memory usage with short strings
- SIMD-accelerated string operations using SSE4.2 when available
- Basic string operations (length, copy, concatenate)
- String comparison and search functions with optimized implementations
- Case conversion (to_upper, to_lower) with SIMD acceleration
- String splitting and joining functions
- Memory-safe implementations with bounds checking
- Available as both static and shared library
- Cache-line aligned memory allocation for better performance

## Building the Project

To build the library and test program:

```bash
make
```

This will create both static and shared libraries in the `bin/` directory and compile the test program.

To clean the build artifacts:

```bash
make clean
```

## Installation

To install the library system-wide:

```bash
sudo make install
```

This will install the library to `/usr/local/lib` and the header to `/usr/local/include`.

## Usage

Include the header in your C files:

```c
#include "string_lib.h"
```

When compiling your program, link against the library:

```bash
gcc -o your_program your_program.c -lstring
```

## Development Notes

This project was ENTIRELY developed using AI tools:

- Claude 3.7 Sonnet (Anthropic) - Used for majority of the development
- Claude 3.5 Sonnet (Anthropic) - Used during periods of rate limiting
- Visual Studio Code's Agent edit tool

No manual coding was required at any stage. The AI systems were responsible for:

1. Designing the entire project architecture
2. Implementing all string library functions from scratch with modern optimizations:
   - Small String Optimization (SSO)
   - SIMD-accelerated operations
   - Cache-line aligned memory management
3. Creating the complete Makefile build system
4. Setting up proper library configuration and linking
5. Organizing output files and implementing proper shared library loading
6. Writing this documentation
7. Implementing comprehensive tests and benchmarks:
   - Unit tests for all string operations
   - Memory leak detection
   - Performance comparisons between SSO and regular strings
   - SIMD vs non-SIMD operation benchmarks

The primary goal of this project was to demonstrate the remarkable capabilities of modern AI coding assistants and showcase how far AI has come in software development. This represents the future of coding, where AI can handle complete projects with minimal human guidance, drastically increasing developer productivity and allowing developers to focus on higher-level design decisions rather than implementation details.

This project serves as evidence that AI-assisted development is now capable of handling end-to-end software development tasks, producing professional-quality code that follows best practices and modern standards. The fact that multiple AI models collaborated on this project (Claude 3.7 and 3.5) also demonstrates the robustness and consistency of modern AI coding assistants.

## License

MIT License. See LICENSE file for details.