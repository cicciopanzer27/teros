/**
 * @file ternary_convert.h
 * @brief Binary ↔ Ternary Conversion Header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TERNARY_CONVERT_H
#define TERNARY_CONVERT_H

#include "trit_array.h"

// =============================================================================
// BINARY ↔ TERNARY CONVERSION
// =============================================================================

/** Convert binary integer to balanced ternary */
trit_array_t* int_to_ternary(int n);

/** Convert ternary array to binary integer */
int ternary_to_int(const trit_array_t* ternary);

// =============================================================================
// STRING REPRESENTATIONS
// =============================================================================

/** Convert ternary array to string (e.g., "+-0-+1") */
char* ternary_to_string(const trit_array_t* ternary);

/** Parse ternary from string */
trit_array_t* string_to_ternary(const char* str);

// =============================================================================
// ARITHMETIC WITH CARRY
// =============================================================================

/**
 * Ternary addition with carry
 * Formula: sum = (a + b) mod 3, carry = ⌊(a + b) / 3⌋
 */
trit_t ternary_add_with_carry(trit_t a, trit_t b, trit_t* carry_out);

/** Add two ternary arrays (positional addition) */
trit_array_t* ternary_add_arrays(const trit_array_t* a, const trit_array_t* b);

// =============================================================================
// MULTIPLICATION
// =============================================================================

/** Ternary multiplication (uses lookup table) */
trit_t ternary_multiply(trit_t a, trit_t b);

/** Multiply ternary array by single trit */
trit_array_t* ternary_shift_multiply(const trit_array_t* a, trit_t b);

/** Full multiplication of two ternary arrays */
trit_array_t* ternary_multiply_arrays(const trit_array_t* a, const trit_array_t* b);

// =============================================================================
// SHIFT OPERATIONS
// =============================================================================

/** Left shift (multiply by 3^n) */
trit_array_t* ternary_shift_left(const trit_array_t* a, size_t n);

/** Right shift (divide by 3^n, integer division) */
trit_array_t* ternary_shift_right(const trit_array_t* a, size_t n);

#endif // TERNARY_CONVERT_H

