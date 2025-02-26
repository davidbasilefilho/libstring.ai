/**
 * @file test_string.c
 * @brief Test program for the custom C23 string library
 * @author GitHub Copilot
 */
#define _POSIX_C_SOURCE 199309L  // For CLOCK_MONOTONIC
#include "string_lib.h"
#include <stdio.h>
#include <assert.h>
#include <time.h>     // For clock functions
#include <unistd.h>   // For POSIX definitions including CLOCK_MONOTONIC

// Utility function to print string info
void print_string_info(const char* label, const String* str) {
    printf("%s: \"%s\" (length: %zu, capacity: %zu)\n", 
           label, string_cstr(str), string_length(str), string_capacity(str));
}

// Test basic string operations
void test_basic_operations() {
    printf("\n=== Basic String Operations ===\n");
    
    // Create a new string
    String* str = string_new("Hello");
    print_string_info("New string", str);
    
    // Append to string
    assert(string_append_cstr(str, ", World!"));
    print_string_info("After append", str);
    
    // Clear string
    string_clear(str);
    print_string_info("After clear", str);
    
    // Set new content
    assert(string_set(str, "Reset content"));
    print_string_info("After set", str);
    
    // Free string
    string_free(str);
    printf("String freed\n");
}

// Test string manipulation functions
void test_manipulation() {
    printf("\n=== String Manipulation ===\n");
    
    // Create test string
    String* str = string_new("  Hello, World!  ");
    print_string_info("Original", str);
    
    // Trim
    string_trim(str);
    print_string_info("After trim", str);
    
    // Convert to upper case
    string_to_upper(str);
    print_string_info("Upper case", str);
    
    // Convert to lower case
    string_to_lower(str);
    print_string_info("Lower case", str);
    
    // Replace substring
    assert(string_replace(str, "world", "C23"));
    print_string_info("After replace", str);
    
    // Free string
    string_free(str);
}

// Test substring operations
void test_substring() {
    printf("\n=== Substring Operations ===\n");
    
    String* str = string_new("Hello, World! Welcome to C23.");
    print_string_info("Original", str);
    
    // Extract substring
    String* sub = string_substr(str, 7, 5);
    print_string_info("Substring", sub);
    
    // Find substring
    ptrdiff_t pos = string_find_cstr(str, "Welcome");
    printf("Found 'Welcome' at position: %td\n", pos);
    
    // Character access
    char c = string_char_at(str, 0);
    printf("Character at position 0: %c\n", c);
    
    // Free strings
    string_free(str);
    string_free(sub);
}

// Test split and join operations
void test_split_join() {
    printf("\n=== Split and Join Operations ===\n");
    
    String* str = string_new("apple,banana,orange,grape");
    print_string_info("Original", str);
    
    // Split string
    size_t count = 0;
    String** parts = string_split(str, ",", &count);
    printf("Split into %zu parts:\n", count);
    
    for (size_t i = 0; i < count; i++) {
        printf("  [%zu]: \"%s\"\n", i, string_cstr(parts[i]));
    }
    
    // Join with a different delimiter
    String* joined = string_join(parts, count, " | ");
    print_string_info("Joined with ' | '", joined);
    
    // Free all strings
    string_free(str);
    string_free(joined);
    
    for (size_t i = 0; i < count; i++) {
        string_free(parts[i]);
    }
    free(parts);
}

// Test error handling and edge cases
void test_edge_cases() {
    printf("\n=== Error Handling and Edge Cases ===\n");
    
    // Empty string
    String* empty = string_new("");
    printf("Is empty string empty? %s\n", string_is_empty(empty) ? "Yes" : "No");
    
    // NULL checks
    printf("string_length(NULL) = %zu\n", string_length(NULL));
    printf("string_is_empty(NULL) = %s\n", string_is_empty(NULL) ? "Yes" : "No");
    
    // Append to empty string
    assert(string_append_cstr(empty, "No longer empty"));
    print_string_info("After append to empty", empty);
    
    // Free string
    string_free(empty);
}

// ========================= BENCHMARK FUNCTIONS =========================

/**
 * Get current time in nanoseconds
 */
static long long get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

/**
 * Print benchmark results
 */
static void print_benchmark_result(const char* operation, long long elapsed_ns, size_t iterations) {
    double elapsed_ms = elapsed_ns / 1000000.0;
    double ops_per_sec = (iterations * 1000.0) / elapsed_ms;
    printf("%-20s | %10.3f ms | %12.0f ops/sec | %8zu iterations\n", 
           operation, elapsed_ms, ops_per_sec, iterations);
}

/**
 * Benchmark string creation and destruction
 */
void benchmark_create_free() {
    const size_t iterations = 100000;
    long long start = get_time_ns();
    
    for (size_t i = 0; i < iterations; i++) {
        String* str = string_new("Hello, World!");
        string_free(str);
    }
    
    long long end = get_time_ns();
    print_benchmark_result("Create/Free", end - start, iterations);
}

/**
 * Benchmark string append operations
 */
void benchmark_append() {
    const size_t iterations = 50000;
    const char* test_str = " additional text";
    long long start = get_time_ns();
    
    String* str = string_new("Initial content");
    for (size_t i = 0; i < iterations; i++) {
        if (!string_append_cstr(str, test_str)) {
            printf("Error: append failed at iteration %zu\n", i);
            break;
        }
    }
    string_free(str);
    
    long long end = get_time_ns();
    print_benchmark_result("Append", end - start, iterations);
}

/**
 * Benchmark string find operations
 */
void benchmark_find() {
    const size_t iterations = 100000;
    String* str = string_new("This is a test string to search for substrings within");
    const char* needle = "string";
    long long start = get_time_ns();
    
    for (size_t i = 0; i < iterations; i++) {
        ptrdiff_t pos = string_find_cstr(str, needle);
        // Using volatile to prevent compiler optimizing away the loop
        volatile ptrdiff_t _ = pos;
        (void)_;
    }
    string_free(str);
    
    long long end = get_time_ns();
    print_benchmark_result("Find", end - start, iterations);
}

/**
 * Benchmark string manipulation operations
 */
void benchmark_manipulations() {
    const size_t iterations = 10000;
    long long start = get_time_ns();
    
    for (size_t i = 0; i < iterations; i++) {
        String* str = string_new("  hello, World! This is a TEST string.  ");
        string_trim(str);
        string_to_upper(str);
        string_to_lower(str);
        if (!string_replace(str, "test", "benchmark")) {
            printf("Error: replace failed at iteration %zu\n", i);
        }
        string_free(str);
    }
    
    long long end = get_time_ns();
    print_benchmark_result("Manipulations", end - start, iterations);
}

/**
 * Benchmark string split and join operations
 */
void benchmark_split_join() {
    const size_t iterations = 10000;
    long long start = get_time_ns();
    
    for (size_t i = 0; i < iterations; i++) {
        String* str = string_new("one,two,three,four,five,six,seven");
        size_t count = 0;
        String** parts = string_split(str, ",", &count);
        
        String* joined = string_join(parts, count, "-");
        
        string_free(str);
        string_free(joined);
        for (size_t j = 0; j < count; j++) {
            string_free(parts[j]);
        }
        free(parts);
    }
    
    long long end = get_time_ns();
    print_benchmark_result("Split/Join", end - start, iterations);
}

/**
 * Run all benchmarks
 */
void run_benchmarks() {
    printf("\n=== STRING LIBRARY BENCHMARKS ===\n");
    printf("%-20s | %10s | %12s | %8s\n", "Operation", "Time (ms)", "Ops/sec", "Iterations");
    printf("---------------------------------------------------------------\n");
    
    benchmark_create_free();
    benchmark_append();
    benchmark_find();
    benchmark_manipulations();
    benchmark_split_join();
}

int main() {
    printf("C23 String Library Test Program\n");
    printf("===============================\n");
    
    // Run tests
    test_basic_operations();
    test_manipulation();
    test_substring();
    test_split_join();
    test_edge_cases();
    
    // Run benchmarks
    run_benchmarks();
    
    printf("\nAll tests and benchmarks completed.\n");
    return 0;
}