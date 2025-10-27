/**
 * @file ternary_alu.h
 * @brief Ternary Arithmetic Logic Unit (ALU) header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TERNARY_ALU_H
#define TERNARY_ALU_H

#include "trit.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// =============================================================================
// TERNARY ALU STRUCTURE
// =============================================================================

typedef struct {
    uint8_t flags;
    bool overflow;
    bool underflow;
    bool zero;
    bool negative;
    bool positive;
} ternary_alu_t;

// =============================================================================
// TERNARY ALU CREATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Create a new ternary ALU
 * @return A new ternary ALU, or NULL on failure
 */
ternary_alu_t* ternary_alu_create(void);

/**
 * @brief Destroy a ternary ALU
 * @param alu The ALU to destroy
 */
void ternary_alu_destroy(ternary_alu_t* alu);

// =============================================================================
// TERNARY ALU ARITHMETIC OPERATIONS
// =============================================================================

/**
 * @brief Add two trits
 * @param alu The ALU instance
 * @param a First trit
 * @param b Second trit
 * @return Result of addition
 */
trit_t ternary_alu_add(ternary_alu_t* alu, trit_t a, trit_t b);

/**
 * @brief Subtract two trits
 * @param alu The ALU instance
 * @param a First trit
 * @param b Second trit
 * @return Result of subtraction
 */
trit_t ternary_alu_subtract(ternary_alu_t* alu, trit_t a, trit_t b);

/**
 * @brief Multiply two trits
 * @param alu The ALU instance
 * @param a First trit
 * @param b Second trit
 * @return Result of multiplication
 */
trit_t ternary_alu_multiply(ternary_alu_t* alu, trit_t a, trit_t b);

/**
 * @brief Divide two trits
 * @param alu The ALU instance
 * @param a First trit
 * @param b Second trit
 * @return Result of division
 */
trit_t ternary_alu_divide(ternary_alu_t* alu, trit_t a, trit_t b);

// =============================================================================
// TERNARY ALU LOGIC OPERATIONS
// =============================================================================

/**
 * @brief Perform AND operation on two trits
 * @param alu The ALU instance
 * @param a First trit
 * @param b Second trit
 * @return Result of AND operation
 */
trit_t ternary_alu_and(ternary_alu_t* alu, trit_t a, trit_t b);

/**
 * @brief Perform OR operation on two trits
 * @param alu The ALU instance
 * @param a First trit
 * @param b Second trit
 * @return Result of OR operation
 */
trit_t ternary_alu_or(ternary_alu_t* alu, trit_t a, trit_t b);

/**
 * @brief Perform NOT operation on a trit
 * @param alu The ALU instance
 * @param a The trit to negate
 * @return Result of NOT operation
 */
trit_t ternary_alu_not(ternary_alu_t* alu, trit_t a);

/**
 * @brief Perform XOR operation on two trits
 * @param alu The ALU instance
 * @param a First trit
 * @param b Second trit
 * @return Result of XOR operation
 */
trit_t ternary_alu_xor(ternary_alu_t* alu, trit_t a, trit_t b);

// =============================================================================
// TERNARY ALU COMPARISON OPERATIONS
// =============================================================================

/**
 * @brief Compare two trits
 * @param alu The ALU instance
 * @param a First trit
 * @param b Second trit
 * @return 1 if equal, 0 if not equal, -1 if unknown
 */
trit_t ternary_alu_compare(ternary_alu_t* alu, trit_t a, trit_t b);

/**
 * @brief Check if first trit is greater than second
 * @param alu The ALU instance
 * @param a First trit
 * @param b Second trit
 * @return 1 if a > b, 0 if a <= b, -1 if unknown
 */
trit_t ternary_alu_greater_than(ternary_alu_t* alu, trit_t a, trit_t b);

/**
 * @brief Check if first trit is less than second
 * @param alu The ALU instance
 * @param a First trit
 * @param b Second trit
 * @return 1 if a < b, 0 if a >= b, -1 if unknown
 */
trit_t ternary_alu_less_than(ternary_alu_t* alu, trit_t a, trit_t b);

// =============================================================================
// TERNARY ALU SHIFT OPERATIONS
// =============================================================================

/**
 * @brief Shift trit left by specified positions
 * @param alu The ALU instance
 * @param a The trit to shift
 * @param positions Number of positions to shift
 * @return Result of shift operation
 */
trit_t ternary_alu_shift_left(ternary_alu_t* alu, trit_t a, int positions);

/**
 * @brief Shift trit right by specified positions
 * @param alu The ALU instance
 * @param a The trit to shift
 * @param positions Number of positions to shift
 * @return Result of shift operation
 */
trit_t ternary_alu_shift_right(ternary_alu_t* alu, trit_t a, int positions);

// =============================================================================
// TERNARY ALU FLAG OPERATIONS
// =============================================================================

/**
 * @brief Get overflow flag
 * @param alu The ALU instance
 * @return true if overflow, false otherwise
 */
bool ternary_alu_get_overflow(ternary_alu_t* alu);

/**
 * @brief Get underflow flag
 * @param alu The ALU instance
 * @return true if underflow, false otherwise
 */
bool ternary_alu_get_underflow(ternary_alu_t* alu);

/**
 * @brief Get zero flag
 * @param alu The ALU instance
 * @return true if zero, false otherwise
 */
bool ternary_alu_get_zero(ternary_alu_t* alu);

/**
 * @brief Get negative flag
 * @param alu The ALU instance
 * @return true if negative, false otherwise
 */
bool ternary_alu_get_negative(ternary_alu_t* alu);

/**
 * @brief Get positive flag
 * @param alu The ALU instance
 * @return true if positive, false otherwise
 */
bool ternary_alu_get_positive(ternary_alu_t* alu);

/**
 * @brief Clear all flags
 * @param alu The ALU instance
 */
void ternary_alu_clear_flags(ternary_alu_t* alu);

// =============================================================================
// TERNARY ALU UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Print ALU flags
 * @param alu The ALU instance
 */
void ternary_alu_print_flags(ternary_alu_t* alu);

/**
 * @brief Debug print ALU information
 * @param alu The ALU instance
 */
void ternary_alu_debug(ternary_alu_t* alu);

#endif // TERNARY_ALU_H
