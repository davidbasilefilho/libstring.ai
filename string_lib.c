#define _POSIX_C_SOURCE 200809L  /* Required for strtok_r */
/**
 * @file string_lib.c
 * @brief Implementation of custom C23 string library
 * @author GitHub Copilot
 */

#include "string_lib.h"
#include <string.h>
#include <ctype.h>

// Initial capacity for new strings
#define INITIAL_CAPACITY 16

/**
 * Helper function to ensure string has enough capacity
 * Returns true if capacity is sufficient or reallocation succeeded
 * Returns false if memory allocation failed
 */
static bool ensure_capacity(String* str, size_t needed_capacity) {
    if (!str) return false;
    
    // If current capacity is enough, return
    if (str->capacity >= needed_capacity) return true;
    
    // Calculate new capacity (double current capacity until sufficient)
    size_t new_capacity = str->capacity;
    while (new_capacity < needed_capacity) {
        new_capacity *= 2;
        
        // Check for overflow
        if (new_capacity < str->capacity) {
            new_capacity = needed_capacity;  // In case of overflow, use exact size
        }
    }
    
    // Reallocate buffer
    char* new_data = realloc(str->data, new_capacity);
    if (!new_data) return false;
    
    // Update string struct
    str->data = new_data;
    str->capacity = new_capacity;
    return true;
}

String* string_new(const char* initial_value) {
    String* str = malloc(sizeof(String));
    if (!str) return NULL;
    
    // Initialize with a default capacity
    str->capacity = INITIAL_CAPACITY;
    str->length = 0;
    str->data = malloc(str->capacity);
    
    if (!str->data) {
        free(str);
        return NULL;
    }
    
    // Initialize with empty string
    str->data[0] = '\0';
    
    // If initial value provided, set it
    if (initial_value) {
        if (!string_set(str, initial_value)) {
            string_free(str);
            return NULL;
        }
    }
    
    return str;
}

String* string_with_capacity(size_t capacity) {
    String* str = malloc(sizeof(String));
    if (!str) return NULL;
    
    // Initialize with the given capacity (or minimum capacity if too small)
    size_t actual_capacity = (capacity < 1) ? INITIAL_CAPACITY : capacity;
    str->capacity = actual_capacity;
    str->length = 0;
    str->data = malloc(str->capacity);
    
    if (!str->data) {
        free(str);
        return NULL;
    }
    
    // Initialize with empty string
    str->data[0] = '\0';
    
    return str;
}

void string_free(String* str) {
    if (!str) return;
    
    // Free the data pointer if it exists
    if (str->data) {
        free(str->data);
    }
    
    // Free the string struct
    free(str);
}

size_t string_length(const String* str) {
    return str ? str->length : 0;
}

size_t string_capacity(const String* str) {
    return str ? str->capacity : 0;
}

const char* string_cstr(const String* str) {
    return str ? str->data : NULL;
}

bool string_is_empty(const String* str) {
    return !str || str->length == 0;
}

bool string_append(String* str, const String* other) {
    if (!str || !other) return false;
    
    return string_append_cstr(str, other->data);
}

bool string_append_cstr(String* str, const char* cstr) {
    if (!str || !cstr) return false;
    
    size_t cstr_len = strlen(cstr);
    
    // Check if we need more capacity
    if (!ensure_capacity(str, str->length + cstr_len + 1)) {
        return false;
    }
    
    // Copy characters to the end of the string
    memcpy(str->data + str->length, cstr, cstr_len + 1);
    
    // Update length
    str->length += cstr_len;
    
    return true;
}

bool string_append_char(String* str, char c) {
    if (!str) return false;
    
    // Check if we need more capacity
    if (!ensure_capacity(str, str->length + 2)) {
        return false;
    }
    
    // Append character and null terminator
    str->data[str->length] = c;
    str->data[str->length + 1] = '\0';
    
    // Update length
    str->length++;
    
    return true;
}

bool string_set(String* str, const char* cstr) {
    if (!str || !cstr) return false;
    
    size_t cstr_len = strlen(cstr);
    
    // Check if we need more capacity
    if (!ensure_capacity(str, cstr_len + 1)) {
        return false;
    }
    
    // Copy the new string
    memcpy(str->data, cstr, cstr_len + 1);
    
    // Update length
    str->length = cstr_len;
    
    return true;
}

void string_clear(String* str) {
    if (!str) return;
    
    // Just set the first character to null terminator
    if (str->data) {
        str->data[0] = '\0';
    }
    
    // Set length to 0
    str->length = 0;
}

int string_compare(const String* str1, const String* str2) {
    if (!str1 && !str2) return 0;
    if (!str1) return -1;
    if (!str2) return 1;
    
    return strcmp(str1->data, str2->data);
}

bool string_equals(const String* str1, const String* str2) {
    return string_compare(str1, str2) == 0;
}

char string_char_at(const String* str, size_t index) {
    if (!str || index >= str->length) return '\0';
    return str->data[index];
}

ptrdiff_t string_find(const String* str, const String* substr) {
    if (!str || !substr) return -1;
    
    return string_find_cstr(str, substr->data);
}

ptrdiff_t string_find_cstr(const String* str, const char* substr) {
    if (!str || !substr) return -1;
    
    char* found = strstr(str->data, substr);
    
    if (!found) return -1;
    
    return found - str->data;
}

String* string_substr(const String* str, size_t start, size_t length) {
    if (!str) return NULL;
    
    // Adjust start and length if they're out of bounds
    if (start >= str->length) {
        start = 0;
        length = 0;
    }
    
    if (start + length > str->length) {
        length = str->length - start;
    }
    
    // Create a new string
    String* result = string_with_capacity(length + 1);
    if (!result) return NULL;
    
    // Copy the substring
    memcpy(result->data, str->data + start, length);
    result->data[length] = '\0';
    result->length = length;
    
    return result;
}

void string_trim(String* str) {
    if (!str || !str->data || str->length == 0) return;
    
    // Find first non-whitespace character from the beginning
    size_t start = 0;
    while (start < str->length && isspace((unsigned char)str->data[start])) {
        start++;
    }
    
    // If the entire string is whitespace
    if (start == str->length) {
        string_clear(str);
        return;
    }
    
    // Find first non-whitespace character from the end
    size_t end = str->length - 1;
    while (end > start && isspace((unsigned char)str->data[end])) {
        end--;
    }
    
    size_t new_length = end - start + 1;
    
    // Shift string to the beginning if needed
    if (start > 0) {
        memmove(str->data, str->data + start, new_length);
    }
    
    // Add null terminator
    str->data[new_length] = '\0';
    str->length = new_length;
}

void string_to_upper(String* str) {
    if (!str || !str->data) return;
    
    for (size_t i = 0; i < str->length; i++) {
        str->data[i] = toupper((unsigned char)str->data[i]);
    }
}

void string_to_lower(String* str) {
    if (!str || !str->data) return;
    
    for (size_t i = 0; i < str->length; i++) {
        str->data[i] = tolower((unsigned char)str->data[i]);
    }
}

String** string_split(const String* str, const char* delim, size_t* count) {
    if (!str || !delim || !count) return NULL;
    
    // Initialize count to 0
    *count = 0;
    
    // Empty string case
    if (str->length == 0) {
        return NULL;
    }
    
    // Count occurrences of delimiter to determine array size
    const char* tmp = str->data;
    const char* token = strstr(tmp, delim);
    size_t delim_len = strlen(delim);
    size_t num_splits = 1;  // At least one substring
    
    while (token) {
        num_splits++;
        tmp = token + delim_len;
        token = strstr(tmp, delim);
    }
    
    // Allocate array for results
    String** result = malloc(num_splits * sizeof(String*));
    if (!result) return NULL;
    
    // Split the string
    char* str_copy = strdup(str->data);  // Create copy for strtok
    if (!str_copy) {
        free(result);
        return NULL;
    }
    
    char* saveptr;
    char* token_str = strtok_r(str_copy, delim, &saveptr);
    size_t i = 0;
    
    while (token_str) {
        result[i] = string_new(token_str);
        if (!result[i]) {
            // Clean up on error
            for (size_t j = 0; j < i; j++) {
                string_free(result[j]);
            }
            free(result);
            free(str_copy);
            return NULL;
        }
        
        i++;
        token_str = strtok_r(NULL, delim, &saveptr);
    }
    
    free(str_copy);
    *count = num_splits;
    return result;
}

String* string_join(String** strs, size_t count, const char* delim) {
    if (!strs || count == 0 || !delim) return NULL;
    
    // Calculate total length
    size_t delim_len = strlen(delim);
    size_t total_len = 0;
    
    for (size_t i = 0; i < count; i++) {
        if (strs[i]) {
            total_len += strs[i]->length;
            
            // Add delimiter length except for the last element
            if (i < count - 1) {
                total_len += delim_len;
            }
        }
    }
    
    // Create new string with calculated capacity
    String* result = string_with_capacity(total_len + 1);
    if (!result) return NULL;
    
    // Join strings
    for (size_t i = 0; i < count; i++) {
        if (strs[i]) {
            string_append(result, strs[i]);
            
            // Add delimiter except after the last element
            if (i < count - 1) {
                string_append_cstr(result, delim);
            }
        }
    }
    
    return result;
}

bool string_replace(String* str, const char* old_str, const char* new_str) {
    if (!str || !old_str || !new_str) return false;
    
    size_t old_len = strlen(old_str);
    size_t new_len = strlen(new_str);
    
    // If the old string is empty, can't replace anything
    if (old_len == 0) return true;
    
    // Create a new temporary buffer for the result
    size_t result_capacity = str->length + 100;  // Extra capacity for growth
    char* result = malloc(result_capacity);
    if (!result) return false;
    
    size_t result_len = 0;
    size_t search_pos = 0;
    
    // Replace all occurrences
    char* found = strstr(str->data + search_pos, old_str);
    
    while (found) {
        size_t pos = found - str->data;  // Calculate position of match
        
        // Ensure enough capacity in result buffer
        if (result_len + (pos - search_pos) + new_len + 1 > result_capacity) {
            result_capacity *= 2;
            char* new_result = realloc(result, result_capacity);
            if (!new_result) {
                free(result);
                return false;
            }
            result = new_result;
        }
        
        // Copy part before the match
        memcpy(result + result_len, str->data + search_pos, pos - search_pos);
        result_len += (pos - search_pos);
        
        // Copy the replacement
        memcpy(result + result_len, new_str, new_len);
        result_len += new_len;
        
        // Update search position
        search_pos = pos + old_len;
        
        // Find next occurrence
        found = strstr(str->data + search_pos, old_str);
    }
    
    // Copy remaining part
    size_t remaining = str->length - search_pos;
    
    // Ensure enough capacity for remaining text
    if (result_len + remaining + 1 > result_capacity) {
        result_capacity = result_len + remaining + 1;
        char* new_result = realloc(result, result_capacity);
        if (!new_result) {
            free(result);
            return false;
        }
        result = new_result;
    }
    
    memcpy(result + result_len, str->data + search_pos, remaining);
    result_len += remaining;
    result[result_len] = '\0';
    
    // Ensure the original string has enough capacity
    if (!ensure_capacity(str, result_len + 1)) {
        free(result);
        return false;
    }
    
    // Replace the original string
    memcpy(str->data, result, result_len + 1);
    str->length = result_len;
    
    free(result);
    return true;
}