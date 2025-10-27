#include <libc/memory.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <trit_math.h>

void* memset(void* dest, int value, size_t count) {
    // Check for null pointer and count > 0
    assert(dest != NULL && count > 0);

    // Cast the void* to a trit pointer
    trit_t* dest_trits = (trit_t*)dest;

    // Initialize the trit with the given value
    trit_init(&value, sizeof(int));

    // Set the memory location to the initialized trit
    for (size_t i = 0; i < count; i++) {
        trit_set(dest_trits + i, &value);
    }

    return dest;
}

void* memcpy(void* dest, const void* src, size_t count) {
    // Check for null pointers and count > 0
    assert(dest != NULL && src != NULL && count > 0);

    // Cast the void* to trit pointers
    trit_t* dest_trits = (trit_t*)dest;
    const trit_t* src_trits = (const trit_t*)src;

    // Copy the memory location to the destination
    for (size_t i = 0; i < count; i++) {
        trit_set(dest_trits + i, src_trits + i);
    }

    return dest;
}

void* memmove(void* dest, const void* src, size_t count) {
    // Check for null pointers and count > 0
    assert(dest != NULL && src != NULL && count > 0);

    // Cast the void* to trit pointers
    trit_t* dest_trits = (trit_t*)dest;
    const trit_t* src_trits = (const trit_t*)src;

    // Check for overlapping memory and adjust the pointers accordingly
    if ((src < dest && src + count > dest) || (src > dest && src < dest + count)) {
        src_trits += count - 1;
        dest_trits += count - 1;
    }

    // Copy the memory location to the destination in reverse order if necessary
    for (size_t i = 0; i < count; i++) {
        trit_set(dest_trits + i, src_trits + i);
    }

    return dest;
}

int memcmp(const void* ptr1, const void* ptr2, size_t count) {
    // Check for null pointers and count > 0
    assert(ptr1 != NULL && ptr2 != NULL && count > 0);

    // Cast the void* to trit pointers
    const trit_t* ptr1_trits = (const trit_t*)ptr1;
    const trit_t* ptr2_trits = (const trit_t*)ptr2;

    for (size_t i = 0; i < count; i++) {
        if (trit_cmp(ptr1_trits + i, ptr2_trits + i) != 0) {
            return trit_cmp(ptr1_trits + i, ptr2_trits + i);
        }
    }

    return 0;
}

void* memchr(const void* ptr, int value, size_t count) {
    // Check for null pointer and count > 0
    assert(ptr != NULL && count > 0);

    // Cast the void* to trit pointers
    const trit_t* ptr_trits = (const trit_t*)ptr;

    // Initialize the trit with the given value
    trit_init(&value, sizeof(int));

    for (size_t i = 0; i < count; i++) {
        if (trit_cmp(ptr_trits + i, &value) == 0) {
            return ptr + i;
        }
    }

    return NULL;
}
```

