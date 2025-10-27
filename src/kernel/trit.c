/**
 * @file trit.c
 * @brief Basic ternary value implementation for TEROS
 * @author TEROS Development Team
 * @date 2025
 */

#include "trit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// =============================================================================
// TRIT IMPLEMENTATION
// =============================================================================

trit_t trit_create(ternary_value_t value) {
    trit_t trit;
    trit.value = value;
    trit.valid = (value >= -1 && value <= 1);
    return trit;
}

ternary_value_t trit_get_value(trit_t trit) {
    return trit.value;
}

void trit_set_value(trit_t* trit, ternary_value_t value) {
    if (trit != NULL) {
        trit->value = value;
        trit->valid = (value >= -1 && value <= 1);
    }
}

bool trit_is_valid(trit_t trit) {
    return trit.valid;
}

// =============================================================================
// TRIT ARITHMETIC OPERATIONS
// =============================================================================

trit_t trit_add(trit_t a, trit_t b) {
    if (!a.valid || !b.valid) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    int result = a.value + b.value;
    if (result > 1) result = 1;
    if (result < -1) result = -1;
    
    return trit_create(result);
}

trit_t trit_subtract(trit_t a, trit_t b) {
    if (!a.valid || !b.valid) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    int result = a.value - b.value;
    if (result > 1) result = 1;
    if (result < -1) result = -1;
    
    return trit_create(result);
}

trit_t trit_multiply(trit_t a, trit_t b) {
    if (!a.valid || !b.valid) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    int result = a.value * b.value;
    if (result > 1) result = 1;
    if (result < -1) result = -1;
    
    return trit_create(result);
}

trit_t trit_divide(trit_t a, trit_t b) {
    if (!a.valid || !b.valid || b.value == 0) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    int result = a.value / b.value;
    if (result > 1) result = 1;
    if (result < -1) result = -1;
    
    return trit_create(result);
}

// =============================================================================
// TRIT LOGIC OPERATIONS
// =============================================================================

trit_t trit_and(trit_t a, trit_t b) {
    if (!a.valid || !b.valid) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    // Ternary AND logic
    if (a.value == -1 || b.value == -1) return trit_create(-1);
    if (a.value == 0 || b.value == 0) return trit_create(0);
    return trit_create(1);
}

trit_t trit_or(trit_t a, trit_t b) {
    if (!a.valid || !b.valid) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    // Ternary OR logic
    if (a.value == 1 || b.value == 1) return trit_create(1);
    if (a.value == 0 || b.value == 0) return trit_create(0);
    return trit_create(-1);
}

trit_t trit_not(trit_t a) {
    if (!a.valid) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    return trit_create(-a.value);
}

trit_t trit_xor(trit_t a, trit_t b) {
    if (!a.valid || !b.valid) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    // Ternary XOR logic
    if (a.value == b.value) return trit_create(0);
    return trit_create(1);
}

// =============================================================================
// TRIT COMPARISON OPERATIONS
// =============================================================================

trit_t trit_equal(trit_t a, trit_t b) {
    if (!a.valid || !b.valid) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    return trit_create(a.value == b.value ? 1 : 0);
}

trit_t trit_greater(trit_t a, trit_t b) {
    if (!a.valid || !b.valid) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    return trit_create(a.value > b.value ? 1 : 0);
}

trit_t trit_less(trit_t a, trit_t b) {
    if (!a.valid || !b.valid) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    return trit_create(a.value < b.value ? 1 : 0);
}

// =============================================================================
// TRIT CONVERSION OPERATIONS
// =============================================================================

int trit_to_int(trit_t trit) {
    if (!trit.valid) return 0;
    return trit.value;
}

float trit_to_float(trit_t trit) {
    if (!trit.valid) return 0.0f;
    return (float)trit.value;
}

bool trit_to_bool(trit_t trit) {
    if (!trit.valid) return false;
    return trit.value > 0;
}

trit_t trit_from_int(int value) {
    if (value > 1) return trit_create(1);
    if (value < -1) return trit_create(-1);
    return trit_create(value);
}

trit_t trit_from_float(float value) {
    if (value > 0.5f) return trit_create(1);
    if (value < -0.5f) return trit_create(-1);
    return trit_create(0);
}

trit_t trit_from_bool(bool value) {
    return trit_create(value ? 1 : -1);
}

// =============================================================================
// TRIT STRING OPERATIONS
// =============================================================================

const char* trit_to_string(trit_t trit) {
    if (!trit.valid) return "UNKNOWN";
    
    switch (trit.value) {
        case -1: return "NEGATIVE";
        case 0:  return "NEUTRAL";
        case 1:  return "POSITIVE";
        default: return "INVALID";
    }
}

trit_t trit_from_string(const char* str) {
    if (str == NULL) return trit_create(TERNARY_UNKNOWN);
    
    if (strcmp(str, "NEGATIVE") == 0 || strcmp(str, "-1") == 0) {
        return trit_create(-1);
    }
    if (strcmp(str, "NEUTRAL") == 0 || strcmp(str, "0") == 0) {
        return trit_create(0);
    }
    if (strcmp(str, "POSITIVE") == 0 || strcmp(str, "1") == 0) {
        return trit_create(1);
    }
    
    return trit_create(TERNARY_UNKNOWN);
}

// =============================================================================
// TRIT BINARY ENCODING
// =============================================================================

uint8_t trit_to_binary(trit_t trit) {
    if (!trit.valid) return 0b00;
    
    switch (trit.value) {
        case -1: return 0b00;  // 00
        case 0:  return 0b01;  // 01
        case 1:  return 0b10;  // 10
        default: return 0b00;
    }
}

trit_t trit_from_binary(uint8_t binary) {
    switch (binary & 0b11) {
        case 0b00: return trit_create(-1);
        case 0b01: return trit_create(0);
        case 0b10: return trit_create(1);
        default:   return trit_create(TERNARY_UNKNOWN);
    }
}

// =============================================================================
// TRIT UTILITY FUNCTIONS
// =============================================================================

void trit_print(trit_t trit) {
    printf("Trit: %s (%d)\n", trit_to_string(trit), trit.value);
}

void trit_debug(trit_t trit) {
    printf("Trit Debug: value=%d, valid=%s, binary=0x%02x\n", 
           trit.value, trit.valid ? "true" : "false", trit_to_binary(trit));
}

bool trit_is_positive(trit_t trit) {
    return trit.valid && trit.value == 1;
}

bool trit_is_negative(trit_t trit) {
    return trit.valid && trit.value == -1;
}

bool trit_is_neutral(trit_t trit) {
    return trit.valid && trit.value == 0;
}

// =============================================================================
// TRIT CONSTANTS
// =============================================================================

const trit_t TRIT_NEGATIVE = {.value = -1, .valid = true};
const trit_t TRIT_NEUTRAL = {.value = 0, .valid = true};
const trit_t TRIT_POSITIVE = {.value = 1, .valid = true};
const trit_t TRIT_UNKNOWN = {.value = 0, .valid = false};
