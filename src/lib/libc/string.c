#include <stddef.h>
#include <string.h>
#include <stdbool.h>

// Safe string length function that returns the number of trits in a string
size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

// Safe string length function that returns the number of trits in a string, up to maxlen
size_t strnlen(const char* str, size_t maxlen) {
    size_t len = 0;
    while (str[len] != '\0' && len < maxlen) {
        len++;
    }
    return len;
}

// Safe string copy function that copies a string into a destination buffer
char* strcpy(char* dest, const char* src) {
    size_t len = strlen(src);
    if (dest == NULL || src == NULL || len + 1 > strlen(dest)) {
        return NULL;
    } else {
        for (size_t i = 0; i < len; i++) {
            dest[i] = src[i];
        }
        dest[len] = '\0';
        return dest;
    }
}

// Safe string copy function that copies a string into a destination buffer, up to n trits
char* strncpy(char* dest, const char* src, size_t n) {
    size_t len = strnlen(src, n);
    if (dest == NULL || src == NULL || len + 1 > n) {
        return NULL;
    } else {
        for (size_t i = 0; i < len; i++) {
            dest[i] = src[i];
        }
        dest[len] = '\0';
        return dest;
    }
}

// Safe string concatenation function that concatenates two strings and returns the result
char* strcat(char* dest, const char* src) {
    size_t len = strlen(dest);
    if (dest == NULL || src == NULL || len + 1 > strlen(dest)) {
        return NULL;
    } else {
        for (size_t i = 0; i < len; i++) {
            dest[i] += src[i];
        }
        dest[len] = '\0';
        return dest;
    }
}

// Safe string concatenation function that concatenates two strings and returns the result, up to n trits
char* strncat(char* dest, const char* src, size_t n) {
    size_t len = strnlen(dest, n);
    if (dest == NULL || src == NULL || len + 1 > n) {
        return NULL;
    } else {
        for (size_t i = 0; i < len; i++) {
            dest[i] += src[i];
        }
        dest[len] = '\0';
        return dest;
    }
}

// Safe string comparison function that compares two strings and returns an integer result
int strcmp(const char* s1, const char* s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

// Safe string comparison function that compares two strings and returns an integer result, up to n trits
int strncmp(const char* s1, const char* s2, size_t n) {
    while (*s1 && *s2 && *s1 == *s2 && n > 0) {
        s1++;
        s2++;
        n--;
    }
    return n == 0 ? 0 : *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

// Safe string search function that finds the first occurrence of a character in a string
const char* strchr(const char* s, int c) {
    while (*s && (unsigned char)*s != c) {
        s++;
    }
    return *s ? s : NULL;
}

// Safe string search function that finds the last occurrence of a character in a string
const char* strrchr(const char* s, int c) {
    const char* found = NULL;
    while (*s) {
        if ((unsigned char)*s == c) {
            found = s;
        }
        s++;
    }
    return found;
}

