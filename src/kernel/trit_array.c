/**
 * @file trit_array.c
 * @brief TritArray implementation for TEROS
 * @author TEROS Development Team
 * @date 2025
 */

#include "trit_array.h"
#include "trit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// =============================================================================
// TRIT ARRAY IMPLEMENTATION
// =============================================================================

trit_array_t* trit_array_create(size_t size) {
    if (size == 0) return NULL;
    
    trit_array_t* array = malloc(sizeof(trit_array_t));
    if (array == NULL) return NULL;
    
    array->trits = malloc(size * sizeof(trit_t));
    if (array->trits == NULL) {
        free(array);
        return NULL;
    }
    
    array->size = size;
    array->capacity = size;
    
    // Initialize all trits to neutral
    for (size_t i = 0; i < size; i++) {
        array->trits[i] = trit_create(TERNARY_NEUTRAL);
    }
    
    return array;
}

void trit_array_destroy(trit_array_t* array) {
    if (array != NULL) {
        if (array->trits != NULL) {
            free(array->trits);
        }
        free(array);
    }
}

trit_t trit_array_get(trit_array_t* array, size_t index) {
    if (array == NULL || array->trits == NULL || index >= array->size) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    return array->trits[index];
}

void trit_array_set(trit_array_t* array, size_t index, trit_t value) {
    if (array == NULL || array->trits == NULL || index >= array->size) {
        return;
    }
    
    array->trits[index] = value;
}

size_t trit_array_size(trit_array_t* array) {
    if (array == NULL) return 0;
    return array->size;
}

bool trit_array_is_valid(trit_array_t* array) {
    if (array == NULL || array->trits == NULL) return false;
    
    for (size_t i = 0; i < array->size; i++) {
        if (!trit_is_valid(array->trits[i])) {
            return false;
        }
    }
    
    return true;
}

// =============================================================================
// TRIT ARRAY OPERATIONS
// =============================================================================

trit_array_t* trit_array_copy(trit_array_t* source) {
    if (source == NULL) return NULL;
    
    trit_array_t* copy = trit_array_create(source->size);
    if (copy == NULL) return NULL;
    
    for (size_t i = 0; i < source->size; i++) {
        copy->trits[i] = source->trits[i];
    }
    
    return copy;
}

trit_array_t* trit_array_concat(trit_array_t* a, trit_array_t* b) {
    if (a == NULL || b == NULL) return NULL;
    
    size_t new_size = a->size + b->size;
    trit_array_t* result = trit_array_create(new_size);
    if (result == NULL) return NULL;
    
    size_t index = 0;
    
    // Copy first array
    for (size_t i = 0; i < a->size; i++) {
        result->trits[index++] = a->trits[i];
    }
    
    // Copy second array
    for (size_t i = 0; i < b->size; i++) {
        result->trits[index++] = b->trits[i];
    }
    
    return result;
}

trit_array_t* trit_array_slice(trit_array_t* array, size_t start, size_t end) {
    if (array == NULL || start >= array->size || end > array->size || start >= end) {
        return NULL;
    }
    
    size_t new_size = end - start;
    trit_array_t* slice = trit_array_create(new_size);
    if (slice == NULL) return NULL;
    
    for (size_t i = 0; i < new_size; i++) {
        slice->trits[i] = array->trits[start + i];
    }
    
    return slice;
}

// =============================================================================
// TRIT ARRAY ARITHMETIC OPERATIONS
// =============================================================================

trit_array_t* trit_array_add(trit_array_t* a, trit_array_t* b) {
    if (a == NULL || b == NULL || a->size != b->size) return NULL;
    
    trit_array_t* result = trit_array_create(a->size);
    if (result == NULL) return NULL;
    
    for (size_t i = 0; i < a->size; i++) {
        result->trits[i] = trit_add(a->trits[i], b->trits[i]);
    }
    
    return result;
}

trit_array_t* trit_array_subtract(trit_array_t* a, trit_array_t* b) {
    if (a == NULL || b == NULL || a->size != b->size) return NULL;
    
    trit_array_t* result = trit_array_create(a->size);
    if (result == NULL) return NULL;
    
    for (size_t i = 0; i < a->size; i++) {
        result->trits[i] = trit_subtract(a->trits[i], b->trits[i]);
    }
    
    return result;
}

trit_array_t* trit_array_multiply(trit_array_t* a, trit_array_t* b) {
    if (a == NULL || b == NULL || a->size != b->size) return NULL;
    
    trit_array_t* result = trit_array_create(a->size);
    if (result == NULL) return NULL;
    
    for (size_t i = 0; i < a->size; i++) {
        result->trits[i] = trit_multiply(a->trits[i], b->trits[i]);
    }
    
    return result;
}

// =============================================================================
// TRIT ARRAY LOGIC OPERATIONS
// =============================================================================

trit_array_t* trit_array_and(trit_array_t* a, trit_array_t* b) {
    if (a == NULL || b == NULL || a->size != b->size) return NULL;
    
    trit_array_t* result = trit_array_create(a->size);
    if (result == NULL) return NULL;
    
    for (size_t i = 0; i < a->size; i++) {
        result->trits[i] = trit_and(a->trits[i], b->trits[i]);
    }
    
    return result;
}

trit_array_t* trit_array_or(trit_array_t* a, trit_array_t* b) {
    if (a == NULL || b == NULL || a->size != b->size) return NULL;
    
    trit_array_t* result = trit_array_create(a->size);
    if (result == NULL) return NULL;
    
    for (size_t i = 0; i < a->size; i++) {
        result->trits[i] = trit_or(a->trits[i], b->trits[i]);
    }
    
    return result;
}

trit_array_t* trit_array_not(trit_array_t* array) {
    if (array == NULL) return NULL;
    
    trit_array_t* result = trit_array_create(array->size);
    if (result == NULL) return NULL;
    
    for (size_t i = 0; i < array->size; i++) {
        result->trits[i] = trit_not(array->trits[i]);
    }
    
    return result;
}

// =============================================================================
// TRIT ARRAY CONVERSION OPERATIONS
// =============================================================================

int trit_array_to_int(trit_array_t* array) {
    if (array == NULL) return 0;
    
    int result = 0;
    int power = 1;
    
    for (int i = array->size - 1; i >= 0; i--) {
        result += trit_to_int(array->trits[i]) * power;
        power *= 3;
    }
    
    return result;
}

trit_array_t* trit_array_from_int(int value, size_t size) {
    trit_array_t* array = trit_array_create(size);
    if (array == NULL) return NULL;
    
    int temp = value;
    for (int i = size - 1; i >= 0; i--) {
        int remainder = temp % 3;
        if (remainder == 2) remainder = -1;
        array->trits[i] = trit_create(remainder);
        temp = (temp - remainder) / 3;
    }
    
    return array;
}

float trit_array_to_float(trit_array_t* array) {
    if (array == NULL) return 0.0f;
    
    float result = 0.0f;
    float power = 1.0f;
    
    for (int i = array->size - 1; i >= 0; i--) {
        result += trit_to_float(array->trits[i]) * power;
        power *= 3.0f;
    }
    
    return result;
}

// =============================================================================
// TRIT ARRAY STRING OPERATIONS
// =============================================================================

char* trit_array_to_string(trit_array_t* array) {
    if (array == NULL) return NULL;
    
    size_t total_length = array->size * 10 + 2; // Estimate
    char* result = malloc(total_length);
    if (result == NULL) return NULL;
    
    strcpy(result, "[");
    
    for (size_t i = 0; i < array->size; i++) {
        const char* trit_str = trit_to_string(array->trits[i]);
        strcat(result, trit_str);
        
        if (i < array->size - 1) {
            strcat(result, ", ");
        }
    }
    
    strcat(result, "]");
    return result;
}

trit_array_t* trit_array_from_string(const char* str) {
    if (str == NULL) return NULL;
    
    // Simple parsing - count commas to determine size
    size_t size = 1;
    for (const char* p = str; *p; p++) {
        if (*p == ',') size++;
    }
    
    trit_array_t* array = trit_array_create(size);
    if (array == NULL) return NULL;
    
    // Parse the string (simplified)
    size_t index = 0;
    const char* start = str;
    
    while (*start && index < size) {
        if (*start == '[' || *start == ' ' || *start == ',') {
            start++;
            continue;
        }
        
        if (*start == ']') break;
        
        // Find next comma or end
        const char* end = start;
        while (*end && *end != ',' && *end != ']') end++;
        
        // Create substring
        size_t len = end - start;
        char* substr = malloc(len + 1);
        if (substr == NULL) {
            trit_array_destroy(array);
            return NULL;
        }
        
        strncpy(substr, start, len);
        substr[len] = '\0';
        
        // Convert to trit
        array->trits[index] = trit_from_string(substr);
        free(substr);
        
        index++;
        start = end;
    }
    
    return array;
}

// =============================================================================
// TRIT ARRAY UTILITY FUNCTIONS
// =============================================================================

void trit_array_print(trit_array_t* array) {
    if (array == NULL) {
        printf("NULL TritArray\n");
        return;
    }
    
    printf("TritArray[%zu]: ", array->size);
    for (size_t i = 0; i < array->size; i++) {
        printf("%s", trit_to_string(array->trits[i]));
        if (i < array->size - 1) printf(", ");
    }
    printf("\n");
}

void trit_array_debug(trit_array_t* array) {
    if (array == NULL) {
        printf("TritArray Debug: NULL\n");
        return;
    }
    
    printf("TritArray Debug: size=%zu, capacity=%zu, valid=%s\n", 
           array->size, array->capacity, trit_array_is_valid(array) ? "true" : "false");
    
    for (size_t i = 0; i < array->size; i++) {
        printf("  [%zu]: ", i);
        trit_debug(array->trits[i]);
    }
}

bool trit_array_equals(trit_array_t* a, trit_array_t* b) {
    if (a == NULL || b == NULL) return false;
    if (a->size != b->size) return false;
    
    for (size_t i = 0; i < a->size; i++) {
        if (trit_get_value(a->trits[i]) != trit_get_value(b->trits[i])) {
            return false;
        }
    }
    
    return true;
}

trit_t trit_array_sum(trit_array_t* array) {
    if (array == NULL) return trit_create(TERNARY_UNKNOWN);
    
    trit_t sum = trit_create(TERNARY_NEUTRAL);
    for (size_t i = 0; i < array->size; i++) {
        sum = trit_add(sum, array->trits[i]);
    }
    
    return sum;
}

trit_t trit_array_product(trit_array_t* array) {
    if (array == NULL) return trit_create(TERNARY_UNKNOWN);
    
    trit_t product = trit_create(TERNARY_POSITIVE);
    for (size_t i = 0; i < array->size; i++) {
        product = trit_multiply(product, array->trits[i]);
    }
    
    return product;
}
