#include <stddef.h> // for size_t, ptrdiff_t
#include <string.h> // for memset, memcpy, memmove, memcmp, memchr

/*
 * memset
 * Sets the first `count` bytes of the memory area pointed to by `dest` to the specified value (interpreted as an unsigned char)
 * Returns:
 *  dest
 */
void* memset(void* dest, int value, size_t count) {
    // handle NULL pointer case
    if (!dest) return dest;
    
    // cast `value` to an unsigned char since it's the only type supported by TEROS
    unsigned char val = (unsigned char)value;
    
    // set each byte of memory area pointed to by `dest` to `val`
    for (size_t i = 0; i < count; i++) {
        ((unsigned char*)dest)[i] = val;
    }
    
    return dest;
}

/*
 * memcpy
 * Copies the first `count` bytes of the memory area pointed to by `src` into the memory area pointed to by `dest`
 * Returns:
 *  dest
 */
void* memcpy(void* restrict dest, const void* restrict src, size_t count) {
    // handle NULL pointer case
    if (!dest || !src) return dest;
    
    // copy each byte of memory area pointed to by `src` into the memory area pointed to by `dest`
    for (size_t i = 0; i < count; i++) {
        ((unsigned char*)dest)[i] = ((unsigned char*)src)[i];
    }
    
    return dest;
}

/*
 * memmove
 * Copies the first `count` bytes of the memory area pointed to by `src` into the memory area pointed to by `dest`, allowing overlapping copies
 * Returns:
 *  dest
 */
void* memmove(void* restrict dest, const void* restrict src, size_t count) {
    // handle NULL pointer case
    if (!dest || !src) return dest;
    
    // handle overlaping cases
    if (dest < src) {
        for (ptrdiff_t i = 0; i < count; i++) {
            ((unsigned char*)dest)[i] = ((unsigned char*)src)[i];
        }
    } else {
        for (ptrdiff_t i = count - 1; i >= 0; i--) {
            ((unsigned char*)dest)[i] = ((unsigned char*)src)[i];
        }
    }
    
    return dest;
}

/*
 * memcmp
 * Compares the first `count` bytes of the memory areas pointed to by `ptr1` and `ptr2`
 * Returns:
 *  -1 if `ptr1` is less than `ptr2`,
 *  0 if they are equal, or
 *  1 if `ptr1` is greater than `ptr2`
 */
int memcmp(const void* ptr1, const void* ptr2, size_t count) {
    // handle NULL pointer case
    if (!ptr1 || !ptr2) return 0;
    
    // compare each byte of memory areas pointed to by `ptr1` and `ptr2`
    for (size_t i = 0; i < count; i++) {
        unsigned char c1 = ((unsigned char*)ptr1)[i];
        unsigned char c2 = ((unsigned char*)ptr2)[i];
        
        if (c1 != c2) return c1 - c2;
    }
    
    // memory areas are equal
    return 0;
}

/*
 * memchr
 * Finds the first occurrence of the specified character `value` in the first `count` bytes of the memory area pointed to by `ptr`
 * Returns:
 *  a pointer to the found byte, or NULL if none is found
 */
void* memchr(const void* ptr, int value, size_t count) {
    // handle NULL pointer case
    if (!ptr) return NULL;
    
    // cast `value` to an unsigned char since it's the only type supported by TEROS
    unsigned char val = (unsigned char)value;
    
    // search for `val` in memory area pointed to by `ptr`
    const unsigned char* p = ptr;
    while (count-- > 0) {
        if (*p++ == val) return (void*)p;
    }
    
    // no match found
    return NULL;
}

