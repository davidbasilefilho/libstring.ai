/**
 * @file string_lib.h
 * @brief Optimized C23 string library implementation
 * @author GitHub Copilot
 */
#ifndef STRING_LIB_H
#define STRING_LIB_H

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Using C2X version check instead of C23
static_assert(__STDC_VERSION__ >= 201710L, "C2X or later is required");

// Small string optimization
#define SSO_SIZE 23  // 23 bytes + null terminator for small strings

/**
 * @brief string structure definition with small string optimization
 */
typedef struct {
    union {
        struct {
            char* data;         // Pointer to string data
            size_t capacity;    // Allocated capacity
        } heap;
        struct {
            char data[SSO_SIZE + 1]; // Inline buffer for small strings
        } stack;
    };
    size_t length;              // Current string length (for both heap and stack)
    uint8_t is_small:1;         // Flag for SSO - 1 if using stack storage
} string;

// Compile-time constants
#define STRING_MAX_LENGTH (SIZE_MAX - 1)

/**
 * @brief Initialize a new string
 * @param initial_value Initial string value (can be NULL)
 * @return New string instance or NULL if allocation fails
 */
[[nodiscard]] string* string_new(const char* initial_value);

/**
 * @brief Create a string with a given capacity
 * @param capacity Initial capacity to allocate
 * @return New string instance or NULL if allocation fails
 */
[[nodiscard]] string* string_with_capacity(size_t capacity);

/**
 * @brief Free a string and its resources
 * @param str string to free
 */
void string_free([[maybe_unused]] string* str);

/**
 * @brief Get string length
 * @param str Target string
 * @return Length of the string
 */
[[nodiscard]] size_t string_length(const string* str);

/**
 * @brief Get string capacity
 * @param str Target string
 * @return Capacity of the string
 */
[[nodiscard]] size_t string_capacity(const string* str);

/**
 * @brief Get C-style string (null-terminated)
 * @param str Target string
 * @return Pointer to null-terminated string
 */
[[nodiscard]] const char* string_cstr(const string* str);

/**
 * @brief Check if string is empty
 * @param str Target string
 * @return true if empty, false otherwise
 */
[[nodiscard]] bool string_is_empty(const string* str);

/**
 * @brief Append string to another
 * @param str Target string
 * @param other string to append
 * @return true if successful, false if allocation fails
 */
[[nodiscard]] bool string_append(string* str, const string* other);

/**
 * @brief Append C-style string to string
 * @param str Target string
 * @param cstr C-style string to append
 * @return true if successful, false if allocation fails
 */
[[nodiscard]] bool string_append_cstr(string* str, const char* cstr);

/**
 * @brief Append character to string
 * @param str Target string 
 * @param c Character to append
 * @return true if successful, false if allocation fails
 */
[[nodiscard]] bool string_append_char(string* str, char c);

/**
 * @brief Set string content
 * @param str Target string
 * @param cstr New string content
 * @return true if successful, false if allocation fails
 */
[[nodiscard]] bool string_set(string* str, const char* cstr);

/**
 * @brief Clear string content (set length to 0)
 * @param str Target string
 */
void string_clear(string* str);

/**
 * @brief Compare two strings
 * @param str1 First string
 * @param str2 Second string
 * @return 0 if equal, <0 if str1 < str2, >0 if str1 > str2
 */
[[nodiscard]] int string_compare(const string* str1, const string* str2);

/**
 * @brief Check if strings are equal
 * @param str1 First string
 * @param str2 Second string
 * @return true if equal, false otherwise
 */
[[nodiscard]] bool string_equals(const string* str1, const string* str2);

/**
 * @brief Get character at index
 * @param str Target string
 * @param index Index of character to get
 * @return Character at index or '\0' if index out of bounds
 */
[[nodiscard]] char string_char_at(const string* str, size_t index);

/**
 * @brief Find substring in string
 * @param str Target string
 * @param substr Substring to find
 * @return Index of first occurrence or -1 if not found
 */
[[nodiscard]] ptrdiff_t string_find(const string* str, const string* substr);

/**
 * @brief Find C-style substring in string
 * @param str Target string
 * @param substr C-style substring to find
 * @return Index of first occurrence or -1 if not found
 */
[[nodiscard]] ptrdiff_t string_find_cstr(const string* str, const char* substr);

/**
 * @brief Create substring
 * @param str Source string
 * @param start Start index
 * @param length Length of substring
 * @return New string instance with substring or NULL if allocation fails
 */
[[nodiscard]] string* string_substr(const string* str, size_t start, size_t length);

/**
 * @brief Trim whitespace from both ends
 * @param str Target string
 */
void string_trim(string* str);

/**
 * @brief Convert string to uppercase
 * @param str Target string
 */
void string_to_upper(string* str);

/**
 * @brief Convert string to lowercase
 * @param str Target string
 */
void string_to_lower(string* str);

/**
 * @brief Split string by delimiter
 * @param str Target string
 * @param delim Delimiter
 * @param count Pointer to store number of splits
 * @return Array of string pointers or NULL if allocation fails
 */
[[nodiscard]] string** string_split(const string* str, const char* delim, size_t* count);

/**
 * @brief Join strings with delimiter
 * @param strs Array of strings
 * @param count Number of strings
 * @param delim Delimiter
 * @return New joined string or NULL if allocation fails
 */
[[nodiscard]] string* string_join(string** strs, size_t count, const char* delim);

/**
 * @brief Replace substring
 * @param str Target string
 * @param old_str Substring to replace
 * @param new_str Replacement
 * @return true if successful, false if allocation fails
 */
[[nodiscard]] bool string_replace(string* str, const char* old_str, const char* new_str);

#endif /* STRING_LIB_H */