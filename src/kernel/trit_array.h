/**
 * @file trit_array.h
 * @brief TritArray header for TEROS
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TRIT_ARRAY_H
#define TRIT_ARRAY_H

#include "trit.h"
#include <stdbool.h>
#include <stddef.h>

// =============================================================================
// TRIT ARRAY STRUCTURE
// =============================================================================

typedef struct {
    trit_t* trits;
    size_t size;
    size_t capacity;
} trit_array_t;

// =============================================================================
// TRIT ARRAY CREATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Create a new trit array with the specified size
 * @param size The number of trits in the array
 * @return A new trit array, or NULL on failure
 */
trit_array_t* trit_array_create(size_t size);

/**
 * @brief Destroy a trit array and free its memory
 * @param array The trit array to destroy
 */
void trit_array_destroy(trit_array_t* array);

// =============================================================================
// TRIT ARRAY ACCESS OPERATIONS
// =============================================================================

/**
 * @brief Get a trit from the array at the specified index
 * @param array The trit array
 * @param index The index of the trit to get
 * @return The trit at the specified index
 */
trit_t trit_array_get(trit_array_t* array, size_t index);

/**
 * @brief Set a trit in the array at the specified index
 * @param array The trit array
 * @param index The index where to set the trit
 * @param value The trit value to set
 */
void trit_array_set(trit_array_t* array, size_t index, trit_t value);

/**
 * @brief Get the size of a trit array
 * @param array The trit array
 * @return The size of the array
 */
size_t trit_array_size(trit_array_t* array);

/**
 * @brief Check if a trit array is valid
 * @param array The trit array to check
 * @return true if valid, false otherwise
 */
bool trit_array_is_valid(trit_array_t* array);

// =============================================================================
// TRIT ARRAY OPERATIONS
// =============================================================================

/**
 * @brief Create a copy of a trit array
 * @param source The source trit array
 * @return A new trit array copy, or NULL on failure
 */
trit_array_t* trit_array_copy(trit_array_t* source);

/**
 * @brief Concatenate two trit arrays
 * @param a First trit array
 * @param b Second trit array
 * @return A new concatenated trit array, or NULL on failure
 */
trit_array_t* trit_array_concat(trit_array_t* a, trit_array_t* b);

/**
 * @brief Create a slice of a trit array
 * @param array The source trit array
 * @param start Start index (inclusive)
 * @param end End index (exclusive)
 * @return A new sliced trit array, or NULL on failure
 */
trit_array_t* trit_array_slice(trit_array_t* array, size_t start, size_t end);

// =============================================================================
// TRIT ARRAY ARITHMETIC OPERATIONS
// =============================================================================

/**
 * @brief Add two trit arrays element-wise
 * @param a First trit array
 * @param b Second trit array
 * @return A new trit array with the result, or NULL on failure
 */
trit_array_t* trit_array_add(trit_array_t* a, trit_array_t* b);

/**
 * @brief Subtract two trit arrays element-wise
 * @param a First trit array
 * @param b Second trit array
 * @return A new trit array with the result, or NULL on failure
 */
trit_array_t* trit_array_subtract(trit_array_t* a, trit_array_t* b);

/**
 * @brief Multiply two trit arrays element-wise
 * @param a First trit array
 * @param b Second trit array
 * @return A new trit array with the result, or NULL on failure
 */
trit_array_t* trit_array_multiply(trit_array_t* a, trit_array_t* b);

// =============================================================================
// TRIT ARRAY LOGIC OPERATIONS
// =============================================================================

/**
 * @brief Perform AND operation on two trit arrays element-wise
 * @param a First trit array
 * @param b Second trit array
 * @return A new trit array with the result, or NULL on failure
 */
trit_array_t* trit_array_and(trit_array_t* a, trit_array_t* b);

/**
 * @brief Perform OR operation on two trit arrays element-wise
 * @param a First trit array
 * @param b Second trit array
 * @return A new trit array with the result, or NULL on failure
 */
trit_array_t* trit_array_or(trit_array_t* a, trit_array_t* b);

/**
 * @brief Perform NOT operation on a trit array element-wise
 * @param array The trit array
 * @return A new trit array with the result, or NULL on failure
 */
trit_array_t* trit_array_not(trit_array_t* array);

// =============================================================================
// TRIT ARRAY CONVERSION OPERATIONS
// =============================================================================

/**
 * @brief Convert trit array to integer
 * @param array The trit array to convert
 * @return Integer value
 */
int trit_array_to_int(trit_array_t* array);

/**
 * @brief Create trit array from integer
 * @param value Integer value
 * @param size Size of the trit array
 * @return A new trit array, or NULL on failure
 */
trit_array_t* trit_array_from_int(int value, size_t size);

/**
 * @brief Convert trit array to float
 * @param array The trit array to convert
 * @return Float value
 */
int32_t trit_array_to_int(trit_array_t* array);

// =============================================================================
// TRIT ARRAY STRING OPERATIONS
// =============================================================================

/**
 * @brief Convert trit array to string representation
 * @param array The trit array to convert
 * @return String representation (must be freed by caller)
 */
char* trit_array_to_string(trit_array_t* array);

/**
 * @brief Create trit array from string
 * @param str String representation
 * @return A new trit array, or NULL on failure
 */
trit_array_t* trit_array_from_string(const char* str);

// =============================================================================
// TRIT ARRAY UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Print trit array to stdout
 * @param array The trit array to print
 */
void trit_array_print(trit_array_t* array);

/**
 * @brief Debug print trit array information
 * @param array The trit array to debug
 */
void trit_array_debug(trit_array_t* array);

/**
 * @brief Check if two trit arrays are equal
 * @param a First trit array
 * @param b Second trit array
 * @return true if equal, false otherwise
 */
bool trit_array_equals(trit_array_t* a, trit_array_t* b);

/**
 * @brief Calculate sum of all trits in the array
 * @param array The trit array
 * @return Sum of all trits
 */
trit_t trit_array_sum(trit_array_t* array);

/**
 * @brief Calculate product of all trits in the array
 * @param array The trit array
 * @return Product of all trits
 */
trit_t trit_array_product(trit_array_t* array);

#endif // TRIT_ARRAY_H
