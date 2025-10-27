#include <stdlib.h>
#include <string.h>
// Note: stdio.h removed - not needed for string functions

/* TEROS specific includes */
#include "trit.h"  // Fixed: was "trits.h" (typo)

// Forward declarations needed for strdup
extern void* malloc(size_t size);
extern void* memcpy(void* dest, const void* src, size_t n);

/**
 * Implementation of strlen for TEROS operating system.
 * This function returns the length of a string in trits.
 *
 * @param str Pointer to the string.
 * @return The length of the string in trits.
 */
size_t strlen(const char* str) {
    size_t len = 0;
    while (*str++ != '\0') {
        len++;
    }
    return len;
}

/**
 * Implementation of strnlen for TEROS operating system.
 * This function returns the length of a string in trits, but no more than maxlen.
 *
 * @param str Pointer to the string.
 * @param maxlen Maximum length of the string.
 * @return The length of the string in trits, but no more than maxlen.
 */
size_t strnlen(const char* str, size_t maxlen) {
    size_t len = 0;
    while (*str != '\0' && len < maxlen) {
        len++;
        str++;
    }
    return len;
}

/**
 * Implementation of strcpy for TEROS operating system.
 * This function copies a string from one location to another, returning the destination pointer.
 *
 * @param dest Pointer to the destination string.
 * @param src Pointer to the source string.
 * @return The destination pointer.
 */
char* strcpy(char* dest, const char* src) {
    size_t i = 0;
    while ((dest[i] = src[i]) != '\0') {
        i++;
    }
    return dest;
}

/**
 * Implementation of strncpy for TEROS operating system.
 * This function copies a string from one location to another, but no more than n characters.
 *
 * @param dest Pointer to the destination string.
 * @param src Pointer to the source string.
 * @param n Number of characters to copy.
 * @return The destination pointer.
 */
char* strncpy(char* dest, const char* src, size_t n) {
    size_t i = 0;
    while (i < n && (dest[i] = src[i]) != '\0') {
        i++;
    }
    return dest;
}

/**
 * Implementation of strcat for TEROS operating system.
 * This function concatenates two strings, returning the destination pointer.
 *
 * @param dest Pointer to the destination string.
 * @param src Pointer to the source string.
 * @return The destination pointer.
 */
char* strcat(char* dest, const char* src) {
    size_t i = 0;
    while (dest[i] != '\0') {
        i++;
    }
    while ((dest[i] = src[i]) != '\0') {
        i++;
    }
    return dest;
}

/**
 * Implementation of strncat for TEROS operating system.
 * This function concatenates two strings, but no more than n characters.
 *
 * @param dest Pointer to the destination string.
 * @param src Pointer to the source string.
 * @param n Number of characters to copy.
 * @return The destination pointer.
 */
char* strncat(char* dest, const char* src, size_t n) {
    size_t i = 0;
    while (dest[i] != '\0') {
        i++;
    }
    while (i < n && (dest[i] = src[i]) != '\0') {
        i++;
    }
    return dest;
}

/**
 * Implementation of strcmp for TEROS operating system.
 * This function compares two strings, returning an integer less than, equal to or greater than 0 depending on whether the first string is found before, equal to or after the second string.
 *
 * @param s1 Pointer to the first string.
 * @param s2 Pointer to the second string.
 * @return An integer less than, equal to or greater than 0 indicating whether the first string is found before, equal to or after the second string.
 */
int strcmp(const char* s1, const char* s2) {
    while (*s1 == *s2 && *s1 != '\0') {
        s1++;
        s2++;
    }
    return (int)(*s1 - *s2);
}

/**
 * Implementation of strncmp for TEROS operating system.
 * This function compares two strings, but no more than n characters, returning an integer less than, equal to or greater than 0 depending on whether the first string is found before, equal to or after the second string.
 *
 * @param s1 Pointer to the first string.
 * @param s2 Pointer to the second string.
 * @param n Number of characters to compare.
 * @return An integer less than, equal to or greater than 0 indicating whether the first string is found before, equal to or after the second string.
 */
int strncmp(const char* s1, const char* s2, size_t n) {
    while (n-- > 0 && *s1 == *s2 && *s1 != '\0') {
        s1++;
        s2++;
    }
    return (int)(*s1 - *s2);
}

/**
 * Implementation of strchr for TEROS operating system.
 * This function returns a pointer to the first occurrence of c in s, or NULL if it is not found.
 *
 * @param s Pointer to the string.
 * @param c The character to search for.
 * @return A pointer to the first occurrence of c in s, or NULL if it is not found.
 */
char* strchr(const char* s, int c) {
    while (*s != '\0') {
        if (*s == (char)c) {
            return (char*)s;  // Cast away const - standard behavior
        }
        s++;
    }
    return NULL;
}

/**
 * Implementation of strrchr for TEROS operating system.
 * This function returns a pointer to the last occurrence of c in s, or NULL if it is not found.
 *
 * @param s Pointer to the string.
 * @param c The character to search for.
 * @return A pointer to the last occurrence of c in s, or NULL if it is not found.
 */
char* strrchr(const char* s, int c) {
    const char* last = NULL;
    while (*s != '\0') {
        if (*s == (char)c) {
            last = s;
        }
        s++;
    }
    return (char*)last;  // Cast away const - standard behavior
}

/**
 * Implementation of strdup for TEROS operating system.
 * Duplicates a string by allocating memory and copying.
 * NOTE: 's' has __attribute__((nonnull)) so NULL check is not needed
 */
char* strdup(const char* s) {
    size_t len = strlen(s) + 1;
    char* dup = (char*)malloc(len);
    if (!dup) return NULL;
    memcpy(dup, s, len);
    return dup;
}

/**
 * Implementation of strtok for TEROS operating system.
 * Tokenizes a string based on delimiters.
 */
char* strtok(char* str, const char* delim) {
    static char* saved = NULL;
    if (str) saved = str;
    if (!saved) return NULL;
    
    // Skip leading delimiters
    while (*saved && strchr(delim, *saved)) saved++;
    if (!*saved) return NULL;
    
    char* token = saved;
    // Find next delimiter
    while (*saved && !strchr(delim, *saved)) saved++;
    if (*saved) *saved++ = '\0';
    
    return token;
}

/**
 * Implementation of strtol for TEROS operating system.
 * Converts string to long integer.
 */
long strtol(const char* nptr, char** endptr, int base) {
    const char* s = nptr;
    long result = 0;
    int sign = 1;
    
    // Skip whitespace
    while (*s == ' ' || *s == '\t' || *s == '\n') s++;
    
    // Handle sign
    if (*s == '-') {
        sign = -1;
        s++;
    } else if (*s == '+') {
        s++;
    }
    
    // Auto-detect base
    if (base == 0) {
        if (*s == '0') {
            s++;
            if (*s == 'x' || *s == 'X') {
                base = 16;
                s++;
            } else {
                base = 8;
            }
        } else {
            base = 10;
        }
    } else if (base == 16 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        s += 2;
    }
    
    // Convert digits
    while (*s) {
        int digit;
        if (*s >= '0' && *s <= '9') {
            digit = *s - '0';
        } else if (*s >= 'a' && *s <= 'z') {
            digit = *s - 'a' + 10;
        } else if (*s >= 'A' && *s <= 'Z') {
            digit = *s - 'A' + 10;
        } else {
            break;
        }
        
        if (digit >= base) break;
        result = result * base + digit;
        s++;
    }
    
    if (endptr) *endptr = (char*)s;
    return sign * result;
}

