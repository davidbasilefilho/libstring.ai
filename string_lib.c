#define _GNU_SOURCE  // For memmem
#define _POSIX_C_SOURCE 200809L
/**
 * @file string_lib.c
 * @brief Optimized C23 string library implementation with SSO
 */
#include "string_lib.h"
#include <string.h>
#include <ctype.h>
#include <errno.h>

// Initial capacity for new strings
#define INITIAL_CAPACITY 16
// Use cache line size for optimal memory alignment
#define CACHE_LINE_SIZE 64

// Helper macros for safer access
#define STRING_DATA(str) ((str)->is_small ? (str)->stack.data : (str)->heap.data)
#define STRING_CAPACITY(str) ((str)->is_small ? SSO_SIZE : (str)->heap.capacity)

// Convert from heap to stack storage if possible
static inline void try_shrink_to_small(string* str) {
    if (!str || str->is_small || str->length > SSO_SIZE) return;
    
    char* old_data = str->heap.data;
    memcpy(str->stack.data, old_data, str->length + 1);
    free(old_data);
    str->is_small = 1;
}

// Convert from stack to heap storage when needed
static inline bool convert_to_heap(string* str, size_t needed_capacity) {
    if (!str) return false;
    if (!str->is_small) return true;  // Already on heap
    
    // Round up to multiple of CACHE_LINE_SIZE for better memory alignment
    size_t new_capacity = (needed_capacity + CACHE_LINE_SIZE - 1) & ~(CACHE_LINE_SIZE - 1);
    
    // Allocate new heap storage
    char* new_data = aligned_alloc(CACHE_LINE_SIZE, new_capacity);
    if (!new_data) {
        errno = ENOMEM;
        return false;
    }
    
    // Copy from stack to heap
    memcpy(new_data, str->stack.data, str->length + 1);
    str->heap.data = new_data;
    str->heap.capacity = new_capacity;
    str->is_small = 0;
    return true;
}

static inline bool ensure_capacity(string* str, size_t needed_capacity) {
    if (!str) return false;
    
    // Check if capacity is already sufficient
    if (STRING_CAPACITY(str) >= needed_capacity) return true;
    
    // Need to switch from stack to heap?
    if (str->is_small) {
        return convert_to_heap(str, needed_capacity);
    }
    
    // Already on heap, just resize
    size_t new_capacity = str->heap.capacity;
    while (new_capacity < needed_capacity) {
        if (__builtin_mul_overflow(new_capacity, 2, &new_capacity)) {
            new_capacity = needed_capacity;
        }
    }
    
    // Round up to multiple of CACHE_LINE_SIZE for better memory alignment
    new_capacity = (new_capacity + CACHE_LINE_SIZE - 1) & ~(CACHE_LINE_SIZE - 1);
    
    char* new_data = realloc(str->heap.data, new_capacity);
    if (!new_data) {
        errno = ENOMEM;
        return false;
    }
    
    str->heap.data = new_data;
    str->heap.capacity = new_capacity;
    return true;
}

string* string_new(const char* initial_value) {
    string* str = malloc(sizeof(string));
    if (!str) return NULL;
    
    // Initialize as small string
    str->is_small = 1;
    str->length = 0;
    str->stack.data[0] = '\0';
    
    if (initial_value) {
        if (!string_set(str, initial_value)) {
            string_free(str);
            return NULL;
        }
    }
    
    return str;
}

string* string_with_capacity(size_t capacity) {
    string* str = malloc(sizeof(string));
    if (!str) return NULL;
    
    if (capacity <= SSO_SIZE) {
        // Initialize as small string
        str->is_small = 1;
        str->length = 0;
        str->stack.data[0] = '\0';
    } else {
        // Initialize as heap string
        size_t actual_capacity = (capacity + CACHE_LINE_SIZE - 1) & ~(CACHE_LINE_SIZE - 1);
        str->heap.data = aligned_alloc(CACHE_LINE_SIZE, actual_capacity);
        
        if (!str->heap.data) {
            free(str);
            errno = ENOMEM;
            return NULL;
        }
        
        str->is_small = 0;
        str->length = 0;
        str->heap.capacity = actual_capacity;
        str->heap.data[0] = '\0';
    }
    
    return str;
}

void string_free(string* str) {
    if (!str) return;
    if (!str->is_small) {
        free(str->heap.data);
    }
    free(str);
}

// Optimized core functions
size_t string_length(const string* str) {
    return str ? str->length : 0;
}

size_t string_capacity(const string* str) {
    return str ? STRING_CAPACITY(str) : 0;
}

const char* string_cstr(const string* str) {
    return str ? STRING_DATA(str) : NULL;
}

bool string_is_empty(const string* str) {
    return !str || str->length == 0;
}

bool string_append(string* str, const string* other) {
    if (!str || !other) return false;
    return string_append_cstr(str, STRING_DATA(other));
}

bool string_append_cstr(string* str, const char* cstr) {
    if (!str || !cstr) return false;
    
    size_t cstr_len = strlen(cstr);
    if (cstr_len == 0) return true;  // Early return for empty strings
    
    if (!ensure_capacity(str, str->length + cstr_len + 1)) {
        return false;
    }
    
    memcpy(STRING_DATA(str) + str->length, cstr, cstr_len + 1);
    str->length += cstr_len;
    return true;
}

bool string_append_char(string* str, char c) {
    if (!str) return false;
    
    if (!ensure_capacity(str, str->length + 2)) {
        return false;
    }
    
    STRING_DATA(str)[str->length] = c;
    STRING_DATA(str)[++str->length] = '\0';
    return true;
}

bool string_set(string* str, const char* cstr) {
    if (!str || !cstr) return false;
    
    size_t cstr_len = strlen(cstr);
    if (!ensure_capacity(str, cstr_len + 1)) {
        return false;
    }
    
    memcpy(STRING_DATA(str), cstr, cstr_len + 1);
    str->length = cstr_len;
    return true;
}

void string_clear(string* str) {
    if (!str) return;
    STRING_DATA(str)[0] = '\0';
    str->length = 0;
}

#ifdef __SSE4_2__
#include <nmmintrin.h>
// SSE4.2 optimized string compare
int string_compare(const string* str1, const string* str2) {
    if (!str1 && !str2) return 0;
    if (!str1) return -1;
    if (!str2) return 1;
    
    // For very short strings, use standard memcmp to avoid overhead
    if (str1->length < 16 || str2->length < 16) {
        size_t min_len = (str1->length < str2->length) ? str1->length : str2->length;
        int result = memcmp(STRING_DATA(str1), STRING_DATA(str2), min_len);
        return result != 0 ? result : (str1->length - str2->length);
    }
    
    size_t len = (str1->length < str2->length) ? str1->length : str2->length;
    size_t i = 0;
    
    // Process 16 bytes at a time using SSE4.2
    while (i + 16 <= len) {
        __m128i xmm1 = _mm_loadu_si128((__m128i*)(STRING_DATA(str1) + i));
        __m128i xmm2 = _mm_loadu_si128((__m128i*)(STRING_DATA(str2) + i));
        
        int result = _mm_cmpestri(
            xmm1, 16,
            xmm2, 16,
            _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT
        );
        
        if (result != 16) {
            // Found a differing character
            return (unsigned char)STRING_DATA(str1)[i + result] - (unsigned char)STRING_DATA(str2)[i + result];
        }
        
        i += 16;
    }
    
    // Handle remaining bytes
    if (i < len) {
        int result = memcmp(STRING_DATA(str1) + i, STRING_DATA(str2) + i, len - i);
        return result != 0 ? result : (str1->length - str2->length);
    }
    
    // Strings are equal up to the minimum length, so the shorter one is less
    return str1->length - str2->length;
}

// SSE4.2 optimized string equality check
bool string_equals(const string* str1, const string* str2) {
    if (str1 == str2) return true;
    if (!str1 || !str2) return false;
    if (str1->length != str2->length) return false;
    
    // For very short strings, use standard memcmp to avoid overhead
    if (str1->length < 16) {
        return memcmp(STRING_DATA(str1), STRING_DATA(str2), str1->length) == 0;
    }
    
    size_t i = 0;
    size_t len = str1->length;
    
    // Process 16 bytes at a time using SSE4.2
    while (i + 16 <= len) {
        __m128i xmm1 = _mm_loadu_si128((__m128i*)(STRING_DATA(str1) + i));
        __m128i xmm2 = _mm_loadu_si128((__m128i*)(STRING_DATA(str2) + i));
        
        int diff = _mm_cmpestri(
            xmm1, 16,
            xmm2, 16,
            _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT
        );
        
        if (diff != 16) {
            return false;  // Found a difference
        }
        
        i += 16;
    }
    
    // Handle remaining bytes
    return memcmp(STRING_DATA(str1) + i, STRING_DATA(str2) + i, len - i) == 0;
}

// SSE4.2 optimized string find
ptrdiff_t string_find_cstr(const string* str, const char* substr) {
    if (!str || !substr) return -1;
    
    size_t substr_len = strlen(substr);
    if (substr_len == 0) return 0;
    if (substr_len > str->length) return -1;
    
    // For very short needles or haystacks, use standard memmem
    if (substr_len < 8 || str->length < 16) {
        char* found = memmem(STRING_DATA(str), str->length, substr, substr_len);
        return found ? (found - STRING_DATA(str)) : -1;
    }
    
    // For longer needles, use SSE4.2 string search
    const int mode = _SIDD_CMP_EQUAL_ORDERED | _SIDD_UBYTE_OPS | _SIDD_LEAST_SIGNIFICANT;
    __m128i needle;
    
    // Load up to 16 bytes of the needle
    if (substr_len >= 16) {
        needle = _mm_loadu_si128((__m128i*)substr);
    } else {
        char temp[16] = {0};
        memcpy(temp, substr, substr_len);
        needle = _mm_loadu_si128((__m128i*)temp);
    }
    
    size_t i = 0;
    while (i <= str->length - substr_len) {
        size_t remaining = str->length - i;
        if (remaining < 16) {
            // Fall back to standard search for the last few bytes
            char* found = memmem(STRING_DATA(str) + i, remaining, substr, substr_len);
            return found ? (found - STRING_DATA(str)) : -1;
        }
        
        __m128i haystack = _mm_loadu_si128((__m128i*)(STRING_DATA(str) + i));
        int index = _mm_cmpestri(needle, substr_len, haystack, 16, mode);
        
        if (index < 16) {
            // Potential match - verify it's complete
            if (i + index + substr_len <= str->length &&
                memcmp(STRING_DATA(str) + i + index, substr, substr_len) == 0) {
                return i + index;
            }
            // Move forward to check after this partial match
            i += index + 1;
        } else {
            // No match in this 16-byte segment
            i += 16 - substr_len + 1;
        }
    }
    
    return -1;
}

// SSE4.2 optimized string to uppercase
void string_to_upper(string* str) {
    if (!str || !str->length) return;
    
    const size_t len = str->length;
    char* data = STRING_DATA(str);
    
    // For very short strings, use standard implementation
    if (len < 16) {
        for (size_t i = 0; i < len; i++) {
            data[i] = toupper((unsigned char)data[i]);
        }
        return;
    }
    
    // Use SIMD to process 16 bytes at a time
    size_t i = 0;
    __m128i lower_a = _mm_set1_epi8('a');
    __m128i lower_z = _mm_set1_epi8('z');
    __m128i to_upper = _mm_set1_epi8('A' - 'a');
    
    for (; i + 16 <= len; i += 16) {
        __m128i chars = _mm_loadu_si128((__m128i*)(data + i));
        
        // Check which chars are in range 'a'..'z'
        __m128i is_lower = _mm_and_si128(
            _mm_cmpgt_epi8(chars, _mm_sub_epi8(lower_a, _mm_set1_epi8(1))),
            _mm_cmpgt_epi8(_mm_add_epi8(lower_z, _mm_set1_epi8(1)), chars)
        );
        
        // Convert to uppercase if in range
        __m128i adjustment = _mm_and_si128(is_lower, to_upper);
        __m128i result = _mm_add_epi8(chars, adjustment);
        
        _mm_storeu_si128((__m128i*)(data + i), result);
    }
    
    // Process remaining characters
    for (; i < len; i++) {
        data[i] = toupper((unsigned char)data[i]);
    }
}

// SSE4.2 optimized string to lowercase
void string_to_lower(string* str) {
    if (!str || !str->length) return;
    
    const size_t len = str->length;
    char* data = STRING_DATA(str);
    
    // For very short strings, use standard implementation
    if (len < 16) {
        for (size_t i = 0; i < len; i++) {
            data[i] = tolower((unsigned char)data[i]);
        }
        return;
    }
    
    // Use SIMD to process 16 bytes at a time
    size_t i = 0;
    __m128i upper_a = _mm_set1_epi8('A');
    __m128i upper_z = _mm_set1_epi8('Z');
    __m128i to_lower = _mm_set1_epi8('a' - 'A');
    
    for (; i + 16 <= len; i += 16) {
        __m128i chars = _mm_loadu_si128((__m128i*)(data + i));
        
        // Check which chars are in range 'A'..'Z'
        __m128i is_upper = _mm_and_si128(
            _mm_cmpgt_epi8(chars, _mm_sub_epi8(upper_a, _mm_set1_epi8(1))),
            _mm_cmpgt_epi8(_mm_add_epi8(upper_z, _mm_set1_epi8(1)), chars)
        );
        
        // Convert to lowercase if in range
        __m128i adjustment = _mm_and_si128(is_upper, to_lower);
        __m128i result = _mm_add_epi8(chars, adjustment);
        
        _mm_storeu_si128((__m128i*)(data + i), result);
    }
    
    // Process remaining characters
    for (; i < len; i++) {
        data[i] = tolower((unsigned char)data[i]);
    }
}
#else
int string_compare(const string* str1, const string* str2) {
    if (!str1 && !str2) return 0;
    if (!str1) return -1;
    if (!str2) return 1;
    return memcmp(STRING_DATA(str1), STRING_DATA(str2), 
                 (str1->length < str2->length) ? str1->length : str2->length);
}

bool string_equals(const string* str1, const string* str2) {
    if (str1 == str2) return true;
    if (!str1 || !str2) return false;
    return str1->length == str2->length && 
           memcmp(STRING_DATA(str1), STRING_DATA(str2), str1->length) == 0;
}

ptrdiff_t string_find_cstr(const string* str, const char* substr) {
    if (!str || !substr) return -1;
    
    char* found = memmem(STRING_DATA(str), str->length, substr, strlen(substr));
    return found ? (found - STRING_DATA(str)) : -1;
}

// Standard implementations for systems without SSE4.2
void string_to_upper(string* str) {
    if (!str || !str->length) return;
    
    char* data = STRING_DATA(str);
    for (size_t i = 0; i < str->length; i++) {
        data[i] = toupper((unsigned char)data[i]);
    }
}

void string_to_lower(string* str) {
    if (!str || !str->length) return;
    
    char* data = STRING_DATA(str);
    for (size_t i = 0; i < str->length; i++) {
        data[i] = tolower((unsigned char)data[i]);
    }
}
#endif

// Add an optimized trim function that automatically switches to small string
// optimization when possible
void string_trim(string* str) {
    if (!str || !str->length) return;
    
    char *start = STRING_DATA(str);
    char *end = STRING_DATA(str) + str->length - 1;
    
    while (start <= end && isspace((unsigned char)*start)) start++;
    while (end > start && isspace((unsigned char)*end)) end--;
    
    size_t new_length = end - start + 1;
    
    // Check if we can convert to small string after trimming
    if (!str->is_small && new_length <= SSO_SIZE) {
        // Convert to small string
        char* heap_data = str->heap.data;
        char temp_buf[SSO_SIZE + 1];
        
        // Copy trimmed data to a temporary buffer
        memcpy(temp_buf, start, new_length);
        temp_buf[new_length] = '\0';
        
        // Convert to small string
        str->is_small = 1;
        memcpy(str->stack.data, temp_buf, new_length + 1);
        
        // Free heap data
        free(heap_data);
    } else if (start > STRING_DATA(str)) {
        // Move the data in place
        memmove(STRING_DATA(str), start, new_length);
    }
    
    STRING_DATA(str)[new_length] = '\0';
    str->length = new_length;
    
    // Try to shrink memory usage
    if (!str->is_small && str->heap.capacity > new_length * 2 && 
        new_length > SSO_SIZE && new_length < 1024) {
        
        // Shrink the buffer to avoid wasting memory
        size_t new_capacity = (new_length * 2 + CACHE_LINE_SIZE - 1) & ~(CACHE_LINE_SIZE - 1);
        char* new_data = realloc(str->heap.data, new_capacity);
        
        if (new_data) {
            str->heap.data = new_data;
            str->heap.capacity = new_capacity;
        }
    }
}

string* string_substr(const string* str, size_t start, size_t length) {
    if (!str || start >= str->length) return NULL;
    
    length = (start + length > str->length) ? (str->length - start) : length;
    string* result = string_with_capacity(length + 1);
    if (!result) return NULL;
    
    memcpy(STRING_DATA(result), STRING_DATA(str) + start, length);
    STRING_DATA(result)[length] = '\0';
    result->length = length;
    return result;
}

string** string_split(const string* str, const char* delim, size_t* count) {
    if (!str || !delim || !count || !str->length) {
        if (count) *count = 0;
        return NULL;
    }
    
    size_t delim_len = strlen(delim);
    if (!delim_len) {
        if (count) *count = 0;
        return NULL;
    }
    
    // Count splits
    size_t num_splits = 1;
    const char* pos = STRING_DATA(str);
    while ((pos = memmem(pos, str->length - (pos - STRING_DATA(str)), delim, delim_len))) {
        num_splits++;
        pos += delim_len;
    }
    
    string** result = calloc(num_splits, sizeof(string*));
    if (!result) {
        if (count) *count = 0;
        return NULL;
    }
    
    // Split string
    const char* start = STRING_DATA(str);
    size_t i = 0;
    
    while (start < STRING_DATA(str) + str->length) {
        const char* end = memmem(start, str->length - (start - STRING_DATA(str)), delim, delim_len);
        if (!end) end = STRING_DATA(str) + str->length;
        
        size_t part_len = end - start;
        result[i] = string_with_capacity(part_len + 1);
        if (!result[i]) {
            for (size_t j = 0; j < i; j++) string_free(result[j]);
            free(result);
            if (count) *count = 0;
            return NULL;
        }
        
        memcpy(STRING_DATA(result[i]), start, part_len);
        STRING_DATA(result[i])[part_len] = '\0';
        result[i]->length = part_len;
        
        start = end + delim_len;
        i++;
        if (!end) break;
    }
    
    *count = num_splits;
    return result;
}

string* string_join(string** strs, size_t count, const char* delim) {
    if (!strs || !count || !delim) return NULL;
    
    size_t delim_len = strlen(delim);
    size_t total_len = 0;
    
    for (size_t i = 0; i < count; i++) {
        if (strs[i]) {
            if (__builtin_add_overflow(total_len, strs[i]->length, &total_len)) {
                errno = EOVERFLOW;
                return NULL;
            }
            if (i < count - 1 && __builtin_add_overflow(total_len, delim_len, &total_len)) {
                errno = EOVERFLOW;
                return NULL;
            }
        }
    }
    
    string* result = string_with_capacity(total_len + 1);
    if (!result) return NULL;
    
    for (size_t i = 0; i < count; i++) {
        if (strs[i]) {
            if (!string_append(result, strs[i])) {
                string_free(result);
                return NULL;
            }
            if (i < count - 1 && !string_append_cstr(result, delim)) {
                string_free(result);
                return NULL;
            }
        }
    }
    
    return result;
}

bool string_replace(string* str, const char* old_str, const char* new_str) {
    if (!str || !old_str || !new_str || !str->length) return false;
    
    size_t old_len = strlen(old_str);
    if (!old_len) return true;
    
    size_t new_len = strlen(new_str);
    const char* pos = STRING_DATA(str);
    size_t count = 0;
    
    // Count occurrences and check for overflow
    while ((pos = memmem(pos, str->length - (pos - STRING_DATA(str)), old_str, old_len))) {
        count++;
        pos += old_len;
    }
    
    if (!count) return true;
    
    // Optimize for the case when new_len <= old_len (in-place replacement)
    if (new_len <= old_len) {
        char* write_pos = STRING_DATA(str);
        const char* read_pos = STRING_DATA(str);
        const char* end = STRING_DATA(str) + str->length;
        
        while (read_pos < end) {
            const char* match = memmem(read_pos, end - read_pos, old_str, old_len);
            
            if (!match) {
                // No more matches, copy remaining data
                size_t remaining = end - read_pos;
                if (write_pos != read_pos) {
                    memmove(write_pos, read_pos, remaining);
                }
                write_pos += remaining;
                break;
            }
            
            // Copy data before match
            size_t prefix_len = match - read_pos;
            if (prefix_len > 0) {
                if (write_pos != read_pos) {
                    memmove(write_pos, read_pos, prefix_len);
                }
                write_pos += prefix_len;
            }
            
            // Insert replacement
            memcpy(write_pos, new_str, new_len);
            write_pos += new_len;
            
            // Move read pointer past the match
            read_pos = match + old_len;
        }
        
        // Null terminate and set new length
        *write_pos = '\0';
        str->length = write_pos - STRING_DATA(str);
        
        // Try to shrink to small string if possible
        if (!str->is_small && str->length <= SSO_SIZE) {
            try_shrink_to_small(str);
        }
        
        return true;
    }
    
    // Calculate new length with overflow checking
    size_t new_total_len;
    if (__builtin_mul_overflow(count, new_len, &new_total_len) ||
        __builtin_add_overflow(str->length, new_total_len, &new_total_len) ||
        __builtin_sub_overflow(new_total_len, count * old_len, &new_total_len)) {
        errno = EOVERFLOW;
        return false;
    }
    
    if (!ensure_capacity(str, new_total_len + 1)) return false;
    
    // Perform replacement from end to beginning
    char* write_pos = STRING_DATA(str) + new_total_len;
    const char* read_pos = STRING_DATA(str) + str->length;
    const char* last_match = NULL;
    
    *write_pos = '\0';
    while (read_pos > STRING_DATA(str)) {
        const char* match = memmem(STRING_DATA(str), read_pos - STRING_DATA(str), old_str, old_len);
        if (!match || match == last_match) break;
        
        size_t suffix_len = read_pos - (match + old_len);
        write_pos -= suffix_len;
        memcpy(write_pos, match + old_len, suffix_len);
        
        write_pos -= new_len;
        memcpy(write_pos, new_str, new_len);
        
        read_pos = match;
        last_match = match;
    }
    
    if (read_pos > STRING_DATA(str)) {
        memmove(STRING_DATA(str), STRING_DATA(str), read_pos - STRING_DATA(str));
    }
    
    str->length = new_total_len;
    return true;
}

char string_char_at(const string* str, size_t index) {
    if (!str || index >= str->length) return '\0';
    return STRING_DATA(str)[index];
}