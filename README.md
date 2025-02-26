# String Library for C

A simple and efficient string manipulation library for C, providing common string operations with a clean API. The library features Small String Optimization (SSO) and SIMD-accelerated operations where available. Built using the C23 standard to leverage modern language features and demonstrate AI's understanding of cutting-edge programming standards.

**Note: This project was ENTIRELY created using AI models - specifically Claude 3.7 Sonnet (for the majority of the work) and Claude 3.5 Sonnet (used during rate limiting periods) from Anthropic, along with Visual Studio Code's Agent edit tool. No manual coding was required.**

## Project Overview

This project implements a modern, optimized string library for C that provides common string manipulation functions. The code is fully compliant with the C23 standard, taking advantage of modern features such as:

- `[[nodiscard]]` attributes for better error handling
- `[[maybe_unused]]` attributes for improved code clarity
- `static_assert` for compile-time checks
- Built-in overflow checking with `__builtin_mul_overflow`
- Modern memory alignment requirements

The code is organized as follows:

- `string_lib.h` - Header file with function declarations and documentation
- `string_lib.c` - Implementation of the string functions
- `test_string.c` - Test program demonstrating the library's usage
- `Makefile` - Build configuration for compiling the library and tests

Build artifacts are stored in the `bin/` directory:
- `bin/libstring.so` - Shared library
- `bin/libstring.a` - Static library
- `bin/test_string` - Test executable

## Features

- Written in modern C23 for enhanced safety and performance
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

The project requires a C23-compatible compiler (such as GCC 13+ or Clang 16+) to build:

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

## Benchmarks

### Test System Specifications
- CPU: AMD Ryzen 7 5700X
- Memory: 2x8GB DDR4-3200MHz CL16

### Performance Results
```
Operation            |  Time (ms)    |      Ops/sec         | Iterations
---------------------------------------------------------------------------------
Create/Free          |      1.544 ms |     64785092 ops/sec |   100000 iterations
Append               |      0.672 ms |     74424365 ops/sec |    50000 iterations
Find                 |      2.187 ms |     45720389 ops/sec |   100000 iterations
Manipulations        |      1.869 ms |      5349507 ops/sec |    10000 iterations
Split/Join           |      2.592 ms |      3858123 ops/sec |    10000 iterations
```

The benchmarks demonstrate the library's high performance, with basic operations like string creation and append achieving over 64 million operations per second. Even more complex operations like string manipulations and split/join operations maintain impressive throughput of several million operations per second.

## Development Notes

This project was ENTIRELY developed using AI tools:

- Claude 3.7 Sonnet (Anthropic) - Used for majority of the development
- Claude 3.5 Sonnet (Anthropic) - Used during periods of rate limiting
- Visual Studio Code's Agent edit tool

No manual coding was required at any stage. The AI systems demonstrated advanced knowledge of modern C programming by:

1. Leveraging C23 features for enhanced safety and performance:
   - Using `[[nodiscard]]` attributes to prevent ignored return values
   - Employing `static_assert` for compile-time checks
   - Utilizing built-in overflow checking functions
   - Implementing proper memory alignment for modern architectures
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

The primary goal of this project was to demonstrate the remarkable capabilities of modern AI coding assistants and showcase how far AI has come in software development. By choosing C23 over older standards, we demonstrate that AI can not only write code but also understand and implement modern best practices and cutting-edge language features. This represents the future of coding, where AI can handle complete projects with minimal human guidance, drastically increasing developer productivity and allowing developers to focus on higher-level design decisions rather than implementation details.

This project serves as evidence that AI-assisted development is now capable of handling end-to-end software development tasks, producing professional-quality code that follows best practices and modern standards. The fact that multiple AI models collaborated on this project (Claude 3.7 and 3.5) also demonstrates the robustness and consistency of modern AI coding assistants, particularly in their understanding of modern programming standards like C23.

## License

[Apache 2.0 License](LICENSE)
