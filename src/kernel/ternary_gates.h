/**
 * @file ternary_gates.h
 * @brief Ternary Logic Gates Interface
 * @author TEROS Development Team
 * @date 2025
 * 
 * This header provides access to all 19,683 dyadic ternary functions
 * and 27 monadic ternary functions via lookup tables.
 */

#ifndef TERNARY_GATES_H
#define TERNARY_GATES_H

#include "trit.h"
#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// CONSTANTS
// =============================================================================

#define T3_DYADIC_FUNCTION_COUNT  19683
#define T3_MONADIC_FUNCTION_COUNT 27
#define T3_MAX_GATE_ID            19682

// Input combination indices (for lookup)
#define T3_INPUT_NEG_NEG 0  // (-1, -1)
#define T3_INPUT_NEG_ZER 1  // (-1,  0)
#define T3_INPUT_NEG_POS 2  // (-1, +1)
#define T3_INPUT_ZER_NEG 3  // ( 0, -1)
#define T3_INPUT_ZER_ZER 4  // ( 0,  0)
#define T3_INPUT_ZER_POS 5  // ( 0, +1)
#define T3_INPUT_POS_NEG 6  // (+1, -1)
#define T3_INPUT_POS_ZER 7  // (+1,  0)
#define T3_INPUT_POS_POS 8  // (+1, +1)

// Well-known function IDs (auto-generated)
extern const uint32_t T3_GATE_KLEENE_AND;
extern const uint32_t T3_GATE_KLEENE_OR;
extern const uint32_t T3_GATE_CONSENSUS;
extern const uint32_t T3_GATE_MINORITY;
extern const uint32_t T3_GATE_PLUS;
extern const uint32_t T3_GATE_TIMES;

// =============================================================================
// DATA STRUCTURES
// =============================================================================

// Metadata for a ternary gate function
typedef struct {
    uint16_t id;
    const char* name;
    const char* description;
    bool is_commutative;
    bool is_associative;
    int8_t identity_element;  // -2 if no identity exists
    uint8_t post_class;
} ternary_gate_info_t;

// =============================================================================
// LOOKUP TABLES
// =============================================================================

// All 19,683 dyadic ternary functions
// Access: TERNARY_DYADIC_GATES[function_id][input_index]
extern const int8_t TERNARY_DYADIC_GATES[19683][9];

// All 27 monadic ternary functions
// Access: TERNARY_MONADIC_GATES[function_id][input_value+1]
extern const int8_t TERNARY_MONADIC_GATES[27][3];

// Statistics
extern const uint32_t T3_DYADIC_FUNCTION_COUNT;
extern const uint32_t T3_MONADIC_FUNCTION_COUNT;
extern const uint32_t T3_DYADIC_TABLE_SIZE;
extern const uint32_t T3_MONADIC_TABLE_SIZE;
extern const uint32_t T3_TOTAL_TABLE_SIZE;

// =============================================================================
// API FUNCTIONS
// =============================================================================

/**
 * @brief Evaluate a dyadic ternary gate
 * @param gate_id Function ID [0..19682]
 * @param a First input trit
 * @param b Second input trit
 * @return Output trit
 */
trit_t ternary_gate_eval(uint16_t gate_id, trit_t a, trit_t b);

/**
 * @brief Evaluate a monadic ternary gate
 * @param gate_id Function ID [0..26]
 * @param a Input trit
 * @return Output trit
 */
trit_t ternary_gate_monadic(uint8_t gate_id, trit_t a);

/**
 * @brief Get metadata for a gate function
 * @param gate_id Function ID
 * @param info Output structure to fill
 * @return true if gate_id is valid
 */
bool ternary_gate_get_info(uint16_t gate_id, ternary_gate_info_t* info);

/**
 * @brief Check if a dyadic function is commutative
 * @param gate_id Function ID
 * @return true if f(a,b) = f(b,a) for all a,b
 */
bool ternary_gate_is_commutative(uint16_t gate_id);

/**
 * @brief Check if a dyadic function is associative
 * @param gate_id Function ID
 * @return true if f(f(a,b),c) = f(a,f(b,c)) for all a,b,c
 */
bool ternary_gate_is_associative(uint16_t gate_id);

/**
 * @brief Find identity element for a function
 * @param gate_id Function ID
 * @return Identity element (-1, 0, or +1), or -2 if none exists
 */
int8_t ternary_gate_find_identity(uint16_t gate_id);

/**
 * @brief Get the name of a well-known function
 * @param gate_id Function ID
 * @return Name string, or NULL if unknown
 */
const char* ternary_gate_get_name(uint16_t gate_id);

/**
 * @brief Print truth table for a gate
 * @param gate_id Function ID
 */
void ternary_gate_print_truth_table(uint16_t gate_id);

#endif // TERNARY_GATES_H

