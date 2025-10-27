/**
 * @file trit.h
 * @brief Basic ternary value header for TEROS
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TRIT_H
#define TRIT_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// =============================================================================
// TERNARY VALUE DEFINITIONS
// =============================================================================

typedef int ternary_value_t;

#define TERNARY_NEGATIVE -1
#define TERNARY_NEUTRAL  0
#define TERNARY_POSITIVE 1
#define TERNARY_UNKNOWN  0

typedef struct {
    ternary_value_t value;
    bool valid;
} trit_t;

// =============================================================================
// TRIT CREATION AND ACCESS
// =============================================================================

/**
 * @brief Create a new trit with the specified value
 * @param value The ternary value (-1, 0, 1)
 * @return A new trit structure
 */
trit_t trit_create(ternary_value_t value);

/**
 * @brief Get the value of a trit
 * @param trit The trit to get the value from
 * @return The ternary value
 */
ternary_value_t trit_get_value(trit_t trit);

/**
 * @brief Set the value of a trit
 * @param trit Pointer to the trit to modify
 * @param value The new ternary value
 */
void trit_set_value(trit_t* trit, ternary_value_t value);

/**
 * @brief Check if a trit is valid
 * @param trit The trit to check
 * @return true if valid, false otherwise
 */
bool trit_is_valid(trit_t trit);

// =============================================================================
// TRIT ARITHMETIC OPERATIONS
// =============================================================================

/**
 * @brief Add two trits
 * @param a First trit
 * @param b Second trit
 * @return Result of addition
 */
trit_t trit_add(trit_t a, trit_t b);

/**
 * @brief Subtract two trits
 * @param a First trit
 * @param b Second trit
 * @return Result of subtraction
 */
trit_t trit_subtract(trit_t a, trit_t b);

/**
 * @brief Multiply two trits
 * @param a First trit
 * @param b Second trit
 * @return Result of multiplication
 */
trit_t trit_multiply(trit_t a, trit_t b);

/**
 * @brief Divide two trits
 * @param a First trit
 * @param b Second trit
 * @return Result of division
 */
trit_t trit_divide(trit_t a, trit_t b);

// =============================================================================
// TRIT LOGIC OPERATIONS
// =============================================================================

/**
 * @brief Ternary AND operation
 * @param a First trit
 * @param b Second trit
 * @return Result of AND operation
 */
trit_t trit_and(trit_t a, trit_t b);

/**
 * @brief Ternary OR operation
 * @param a First trit
 * @param b Second trit
 * @return Result of OR operation
 */
trit_t trit_or(trit_t a, trit_t b);

/**
 * @brief Ternary NOT operation
 * @param a The trit to negate
 * @return Result of NOT operation
 */
trit_t trit_not(trit_t a);

/**
 * @brief Ternary XOR operation
 * @param a First trit
 * @param b Second trit
 * @return Result of XOR operation
 */
trit_t trit_xor(trit_t a, trit_t b);

// =============================================================================
// TRIT COMPARISON OPERATIONS
// =============================================================================

/**
 * @brief Check if two trits are equal
 * @param a First trit
 * @param b Second trit
 * @return 1 if equal, 0 if not equal, -1 if unknown
 */
trit_t trit_equal(trit_t a, trit_t b);

/**
 * @brief Check if first trit is greater than second
 * @param a First trit
 * @param b Second trit
 * @return 1 if a > b, 0 if a <= b, -1 if unknown
 */
trit_t trit_greater(trit_t a, trit_t b);

/**
 * @brief Check if first trit is less than second
 * @param a First trit
 * @param b Second trit
 * @return 1 if a < b, 0 if a >= b, -1 if unknown
 */
trit_t trit_less(trit_t a, trit_t b);

// =============================================================================
// TRIT CONVERSION OPERATIONS
// =============================================================================

/**
 * @brief Convert trit to integer
 * @param trit The trit to convert
 * @return Integer value
 */
int trit_to_int(trit_t trit);

/**
 * @brief Convert trit to float
 * @param trit The trit to convert
 * @return Float value
 */
int32_t trit_to_int(trit_t trit);

/**
 * @brief Convert trit to boolean
 * @param trit The trit to convert
 * @return Boolean value
 */
bool trit_to_bool(trit_t trit);

/**
 * @brief Create trit from integer
 * @param value Integer value
 * @return New trit
 */
trit_t trit_from_int(int value);

/**
 * @brief Create trit from float
 * @param value Float value
 * @return New trit
 */
trit_t trit_from_float(float value);

/**
 * @brief Create trit from boolean
 * @param value Boolean value
 * @return New trit
 */
trit_t trit_from_bool(bool value);

// =============================================================================
// TRIT STRING OPERATIONS
// =============================================================================

/**
 * @brief Convert trit to string representation
 * @param trit The trit to convert
 * @return String representation
 */
const char* trit_to_string(trit_t trit);

/**
 * @brief Create trit from string
 * @param str String representation
 * @return New trit
 */
trit_t trit_from_string(const char* str);

// =============================================================================
// TRIT BINARY ENCODING
// =============================================================================

/**
 * @brief Convert trit to binary representation (2-bit)
 * @param trit The trit to convert
 * @return Binary representation
 */
uint8_t trit_to_binary(trit_t trit);

/**
 * @brief Create trit from binary representation
 * @param binary Binary representation
 * @return New trit
 */
trit_t trit_from_binary(uint8_t binary);

// =============================================================================
// TRIT UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Print trit to stdout
 * @param trit The trit to print
 */
void trit_print(trit_t trit);

/**
 * @brief Debug print trit information
 * @param trit The trit to debug
 */
void trit_debug(trit_t trit);

/**
 * @brief Check if trit is positive
 * @param trit The trit to check
 * @return true if positive, false otherwise
 */
bool trit_is_positive(trit_t trit);

/**
 * @brief Check if trit is negative
 * @param trit The trit to check
 * @return true if negative, false otherwise
 */
bool trit_is_negative(trit_t trit);

/**
 * @brief Check if trit is neutral
 * @param trit The trit to check
 * @return true if neutral, false otherwise
 */
bool trit_is_neutral(trit_t trit);

// =============================================================================
// TRIT CONSTANTS
// =============================================================================

extern const trit_t TRIT_NEGATIVE;
extern const trit_t TRIT_NEUTRAL;
extern const trit_t TRIT_POSITIVE;
extern const trit_t TRIT_UNKNOWN;

#endif // TRIT_H
