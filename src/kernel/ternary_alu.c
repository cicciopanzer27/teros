/**
 * @file ternary_alu.c
 * @brief Ternary Arithmetic Logic Unit (ALU) implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "ternary_alu.h"
#include "trit.h"
#include "trit_array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// =============================================================================
// TERNARY ALU IMPLEMENTATION
// =============================================================================

ternary_alu_t* ternary_alu_create(void) {
    ternary_alu_t* alu = malloc(sizeof(ternary_alu_t));
    if (alu == NULL) return NULL;
    
    alu->flags = 0;
    alu->overflow = false;
    alu->underflow = false;
    alu->zero = false;
    alu->negative = false;
    alu->positive = false;
    
    return alu;
}

void ternary_alu_destroy(ternary_alu_t* alu) {
    if (alu != NULL) {
        free(alu);
    }
}

// =============================================================================
// TERNARY ALU ARITHMETIC OPERATIONS
// =============================================================================

trit_t ternary_alu_add(ternary_alu_t* alu, trit_t a, trit_t b) {
    if (alu == NULL || !trit_is_valid(a) || !trit_is_valid(b)) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    int result = a.value + b.value;
    
    // Check for overflow/underflow
    if (result > 1) {
        alu->overflow = true;
        result = 1;
    } else if (result < -1) {
        alu->underflow = true;
        result = -1;
    }
    
    trit_t trit_result = trit_create(result);
    
    // Update flags
    alu->zero = (result == 0);
    alu->negative = (result < 0);
    alu->positive = (result > 0);
    
    return trit_result;
}

trit_t ternary_alu_subtract(ternary_alu_t* alu, trit_t a, trit_t b) {
    if (alu == NULL || !trit_is_valid(a) || !trit_is_valid(b)) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    int result = a.value - b.value;
    
    // Check for overflow/underflow
    if (result > 1) {
        alu->overflow = true;
        result = 1;
    } else if (result < -1) {
        alu->underflow = true;
        result = -1;
    }
    
    trit_t trit_result = trit_create(result);
    
    // Update flags
    alu->zero = (result == 0);
    alu->negative = (result < 0);
    alu->positive = (result > 0);
    
    return trit_result;
}

trit_t ternary_alu_multiply(ternary_alu_t* alu, trit_t a, trit_t b) {
    if (alu == NULL || !trit_is_valid(a) || !trit_is_valid(b)) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    int result = a.value * b.value;
    
    // Check for overflow/underflow
    if (result > 1) {
        alu->overflow = true;
        result = 1;
    } else if (result < -1) {
        alu->underflow = true;
        result = -1;
    }
    
    trit_t trit_result = trit_create(result);
    
    // Update flags
    alu->zero = (result == 0);
    alu->negative = (result < 0);
    alu->positive = (result > 0);
    
    return trit_result;
}

trit_t ternary_alu_divide(ternary_alu_t* alu, trit_t a, trit_t b) {
    if (alu == NULL || !trit_is_valid(a) || !trit_is_valid(b)) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    if (trit_is_neutral(b)) {
        alu->overflow = true;
        return trit_create(TERNARY_UNKNOWN);
    }
    
    int result = a.value / b.value;
    
    // Check for overflow/underflow
    if (result > 1) {
        alu->overflow = true;
        result = 1;
    } else if (result < -1) {
        alu->underflow = true;
        result = -1;
    }
    
    trit_t trit_result = trit_create(result);
    
    // Update flags
    alu->zero = (result == 0);
    alu->negative = (result < 0);
    alu->positive = (result > 0);
    
    return trit_result;
}

// =============================================================================
// TERNARY ALU LOGIC OPERATIONS
// =============================================================================

trit_t ternary_alu_and(ternary_alu_t* alu, trit_t a, trit_t b) {
    if (alu == NULL || !trit_is_valid(a) || !trit_is_valid(b)) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t result = trit_and(a, b);
    
    // Update flags
    alu->zero = trit_is_neutral(result);
    alu->negative = trit_is_negative(result);
    alu->positive = trit_is_positive(result);
    
    return result;
}

trit_t ternary_alu_or(ternary_alu_t* alu, trit_t a, trit_t b) {
    if (alu == NULL || !trit_is_valid(a) || !trit_is_valid(b)) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t result = trit_or(a, b);
    
    // Update flags
    alu->zero = trit_is_neutral(result);
    alu->negative = trit_is_negative(result);
    alu->positive = trit_is_positive(result);
    
    return result;
}

trit_t ternary_alu_not(ternary_alu_t* alu, trit_t a) {
    if (alu == NULL || !trit_is_valid(a)) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t result = trit_not(a);
    
    // Update flags
    alu->zero = trit_is_neutral(result);
    alu->negative = trit_is_negative(result);
    alu->positive = trit_is_positive(result);
    
    return result;
}

trit_t ternary_alu_xor(ternary_alu_t* alu, trit_t a, trit_t b) {
    if (alu == NULL || !trit_is_valid(a) || !trit_is_valid(b)) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t result = trit_xor(a, b);
    
    // Update flags
    alu->zero = trit_is_neutral(result);
    alu->negative = trit_is_negative(result);
    alu->positive = trit_is_positive(result);
    
    return result;
}

// =============================================================================
// TERNARY ALU COMPARISON OPERATIONS
// =============================================================================

trit_t ternary_alu_compare(ternary_alu_t* alu, trit_t a, trit_t b) {
    if (alu == NULL || !trit_is_valid(a) || !trit_is_valid(b)) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t result = trit_equal(a, b);
    
    // Update flags
    alu->zero = trit_is_neutral(result);
    alu->negative = trit_is_negative(result);
    alu->positive = trit_is_positive(result);
    
    return result;
}

trit_t ternary_alu_greater_than(ternary_alu_t* alu, trit_t a, trit_t b) {
    if (alu == NULL || !trit_is_valid(a) || !trit_is_valid(b)) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t result = trit_greater(a, b);
    
    // Update flags
    alu->zero = trit_is_neutral(result);
    alu->negative = trit_is_negative(result);
    alu->positive = trit_is_positive(result);
    
    return result;
}

trit_t ternary_alu_less_than(ternary_alu_t* alu, trit_t a, trit_t b) {
    if (alu == NULL || !trit_is_valid(a) || !trit_is_valid(b)) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    trit_t result = trit_less(a, b);
    
    // Update flags
    alu->zero = trit_is_neutral(result);
    alu->negative = trit_is_negative(result);
    alu->positive = trit_is_positive(result);
    
    return result;
}

// =============================================================================
// TERNARY ALU SHIFT OPERATIONS
// =============================================================================

trit_t ternary_alu_shift_left(ternary_alu_t* alu, trit_t a, int positions) {
    if (alu == NULL || !trit_is_valid(a)) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    int result = a.value;
    
    for (int i = 0; i < positions; i++) {
        result *= 3;  // Ternary shift left
        if (result > 1) {
            alu->overflow = true;
            result = 1;
        } else if (result < -1) {
            alu->underflow = true;
            result = -1;
        }
    }
    
    trit_t trit_result = trit_create(result);
    
    // Update flags
    alu->zero = (result == 0);
    alu->negative = (result < 0);
    alu->positive = (result > 0);
    
    return trit_result;
}

trit_t ternary_alu_shift_right(ternary_alu_t* alu, trit_t a, int positions) {
    if (alu == NULL || !trit_is_valid(a)) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    int result = a.value;
    
    for (int i = 0; i < positions; i++) {
        result /= 3;  // Ternary shift right
        if (result > 1) {
            alu->overflow = true;
            result = 1;
        } else if (result < -1) {
            alu->underflow = true;
            result = -1;
        }
    }
    
    trit_t trit_result = trit_create(result);
    
    // Update flags
    alu->zero = (result == 0);
    alu->negative = (result < 0);
    alu->positive = (result > 0);
    
    return trit_result;
}

// =============================================================================
// TERNARY ALU FLAG OPERATIONS
// =============================================================================

bool ternary_alu_get_overflow(ternary_alu_t* alu) {
    return alu != NULL && alu->overflow;
}

bool ternary_alu_get_underflow(ternary_alu_t* alu) {
    return alu != NULL && alu->underflow;
}

bool ternary_alu_get_zero(ternary_alu_t* alu) {
    return alu != NULL && alu->zero;
}

bool ternary_alu_get_negative(ternary_alu_t* alu) {
    return alu != NULL && alu->negative;
}

bool ternary_alu_get_positive(ternary_alu_t* alu) {
    return alu != NULL && alu->positive;
}

void ternary_alu_clear_flags(ternary_alu_t* alu) {
    if (alu != NULL) {
        alu->overflow = false;
        alu->underflow = false;
        alu->zero = false;
        alu->negative = false;
        alu->positive = false;
    }
}

// =============================================================================
// TERNARY ALU UTILITY FUNCTIONS
// =============================================================================

void ternary_alu_print_flags(ternary_alu_t* alu) {
    // DEBUG: printf version disabled (requires printf)
    if (alu == NULL) {
        return;
    }
    // Would print: overflow, underflow, zero, negative, positive flags
    (void)alu;
}

void ternary_alu_debug(ternary_alu_t* alu) {
    // DEBUG: printf version disabled (requires printf)
    if (alu == NULL) {
        return;
    }
    // Would print: flags, overflow, underflow, zero, negative, positive
    (void)alu;
}
