/**
 * @file string_lib.h
 * @brief Custom C23 string library implementation
 * @author GitHub Copilot
 */

#ifndef STRING_LIB_H
#define STRING_LIB_H

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief String structure definition
 */
typedef struct {
    char* data;         // Pointer to string data
    size_t length;      // Current string length
    size_t capacity;    // Allocated capacity
} String;

/**
 * @brief Initialize a new string
 * @param initial_value Initial string value (can be NULL)
 * @return New String instance or NULL if allocation fails
 */
String* string_new(const char* initial_value);

/**
 * @brief Create a string with a given capacity
 * @param capacity Initial capacity to allocate
 * @return New String instance or NULL if allocation fails
 */
String* string_with_capacity(size_t capacity);

/**
 * @brief Free a string and its resources
 * @param str String to free
 */
void string_free(String* str);

/**
 * @brief Get string length
 * @param str Target string
 * @return Length of the string
 */
size_t string_length(const String* str);

/**
 * @brief Get string capacity
 * @param str Target string
 * @return Capacity of the string
 */
size_t string_capacity(const String* str);

/**
 * @brief Get C-style string (null-terminated)
 * @param str Target string
 * @return Pointer to null-terminated string
 */
const char* string_cstr(const String* str);

/**
 * @brief Check if string is empty
 * @param str Target string
 * @return true if empty, false otherwise
 */
bool string_is_empty(const String* str);

/**
 * @brief Append string to another
 * @param str Target string
 * @param other String to append
 * @return true if successful, false if allocation fails
 */
bool string_append(String* str, const String* other);

/**
 * @brief Append C-style string to string
 * @param str Target string
 * @param cstr C-style string to append
 * @return true if successful, false if allocation fails
 */
bool string_append_cstr(String* str, const char* cstr);

/**
 * @brief Append character to string
 * @param str Target string 
 * @param c Character to append
 * @return true if successful, false if allocation fails
 */
bool string_append_char(String* str, char c);

/**
 * @brief Set string content
 * @param str Target string
 * @param cstr New string content
 * @return true if successful, false if allocation fails
 */
bool string_set(String* str, const char* cstr);

/**
 * @brief Clear string content (set length to 0)
 * @param str Target string
 */
void string_clear(String* str);

/**
 * @brief Compare two strings
 * @param str1 First string
 * @param str2 Second string
 * @return 0 if equal, <0 if str1 < str2, >0 if str1 > str2
 */
int string_compare(const String* str1, const String* str2);

/**
 * @brief Check if strings are equal
 * @param str1 First string
 * @param str2 Second string
 * @return true if equal, false otherwise
 */
bool string_equals(const String* str1, const String* str2);

/**
 * @brief Get character at index
 * @param str Target string
 * @param index Index of character to get
 * @return Character at index or '\0' if index out of bounds
 */
char string_char_at(const String* str, size_t index);

/**
 * @brief Find substring in string
 * @param str Target string
 * @param substr Substring to find
 * @return Index of first occurrence or -1 if not found
 */
ptrdiff_t string_find(const String* str, const String* substr);

/**
 * @brief Find C-style substring in string
 * @param str Target string
 * @param substr C-style substring to find
 * @return Index of first occurrence or -1 if not found
 */
ptrdiff_t string_find_cstr(const String* str, const char* substr);

/**
 * @brief Create substring
 * @param str Source string
 * @param start Start index
 * @param length Length of substring
 * @return New String instance with substring or NULL if allocation fails
 */
String* string_substr(const String* str, size_t start, size_t length);

/**
 * @brief Trim whitespace from both ends
 * @param str Target string
 */
void string_trim(String* str);

/**
 * @brief Convert string to uppercase
 * @param str Target string
 */
void string_to_upper(String* str);

/**
 * @brief Convert string to lowercase
 * @param str Target string
 */
void string_to_lower(String* str);

/**
 * @brief Split string by delimiter
 * @param str Target string
 * @param delim Delimiter
 * @param count Pointer to store number of splits
 * @return Array of String pointers or NULL if allocation fails
 */
String** string_split(const String* str, const char* delim, size_t* count);

/**
 * @brief Join strings with delimiter
 * @param strs Array of strings
 * @param count Number of strings
 * @param delim Delimiter
 * @return New joined string or NULL if allocation fails
 */
String* string_join(String** strs, size_t count, const char* delim);

/**
 * @brief Replace substring
 * @param str Target string
 * @param old_str Substring to replace
 * @param new_str Replacement
 * @return true if successful, false if allocation fails
 */
bool string_replace(String* str, const char* old_str, const char* new_str);

#endif /* STRING_LIB_H */