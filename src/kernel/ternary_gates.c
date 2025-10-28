/**
 * @file ternary_gates.c
 * @brief Ternary Logic Gates Implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "ternary_gates.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

// Include the auto-generated lookup tables
#include "ternary_gates_data.c"

// Well-known function constants (from ternary_gates_data.c)
const uint32_t T3_GATE_KLEENE_AND = 15633;
const uint32_t T3_GATE_KLEENE_OR = 19569;
const uint32_t T3_GATE_CONSENSUS = 16371;
const uint32_t T3_GATE_MINORITY = 3311;
const uint32_t T3_GATE_PLUS = 5681;
const uint32_t T3_GATE_TIMES = 15665;

// =============================================================================
// HELPER FUNCTIONS
// =============================================================================

/**
 * Convert trit pair (a,b) to lookup table index
 */
static inline uint8_t trit_pair_to_index(int8_t a, int8_t b)
{
    // Convert {-1, 0, +1} to {0, 1, 2}
    uint8_t a_idx = a + 1;
    uint8_t b_idx = b + 1;
    return a_idx * 3 + b_idx;
}

/**
 * Convert monadic trit value to lookup table index
 */
static inline uint8_t trit_to_index(int8_t a)
{
    return a + 1;  // {-1, 0, +1} -> {0, 1, 2}
}

// =============================================================================
// API IMPLEMENTATION
// =============================================================================

trit_t ternary_gate_eval(uint16_t gate_id, trit_t a, trit_t b)
{
    if (gate_id >= T3_DYADIC_FUNCTION_COUNT) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    if (!trit_is_valid(a) || !trit_is_valid(b)) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    // Get input values
    int8_t a_val = trit_get_value(a);
    int8_t b_val = trit_get_value(b);
    
    // Compute lookup index
    uint8_t index = trit_pair_to_index(a_val, b_val);
    
    // Lookup result
    int8_t result_val = TERNARY_DYADIC_GATES[gate_id][index];
    
    return trit_create(result_val);
}

trit_t ternary_gate_monadic(uint8_t gate_id, trit_t a)
{
    if (gate_id >= T3_MONADIC_FUNCTION_COUNT) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    if (!trit_is_valid(a)) {
        return trit_create(TERNARY_UNKNOWN);
    }
    
    // Get input value
    int8_t a_val = trit_get_value(a);
    
    // Compute lookup index
    uint8_t index = trit_to_index(a_val);
    
    // Lookup result
    int8_t result_val = TERNARY_MONADIC_GATES[gate_id][index];
    
    return trit_create(result_val);
}

bool ternary_gate_get_info(uint16_t gate_id, ternary_gate_info_t* info)
{
    if (info == NULL) {
        return false;
    }
    
    if (gate_id < T3_DYADIC_FUNCTION_COUNT) {
        info->id = gate_id;
        info->name = ternary_gate_get_name(gate_id);
        info->description = "";
        info->is_commutative = ternary_gate_is_commutative(gate_id);
        info->is_associative = ternary_gate_is_associative(gate_id);
        info->identity_element = ternary_gate_find_identity(gate_id);
        info->post_class = 0;  // TODO: Implement Post classification
        return true;
    }
    
    return false;
}

bool ternary_gate_is_commutative(uint16_t gate_id)
{
    if (gate_id >= T3_DYADIC_FUNCTION_COUNT) {
        return false;
    }
    
    // Check all 9 input pairs
    int8_t inputs[] = {-1, 0, 1};
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int8_t a = inputs[i];
            int8_t b = inputs[j];
            
            // f(a, b)
            uint8_t idx1 = trit_pair_to_index(a, b);
            int8_t result1 = TERNARY_DYADIC_GATES[gate_id][idx1];
            
            // f(b, a)
            uint8_t idx2 = trit_pair_to_index(b, a);
            int8_t result2 = TERNARY_DYADIC_GATES[gate_id][idx2];
            
            if (result1 != result2) {
                return false;
            }
        }
    }
    
    return true;
}

bool ternary_gate_is_associative(uint16_t gate_id)
{
    if (gate_id >= T3_DYADIC_FUNCTION_COUNT) {
        return false;
    }
    
    // Check all 3^3 = 27 combinations of (a, b, c)
    int8_t inputs[] = {-1, 0, 1};
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                int8_t a = inputs[i];
                int8_t b = inputs[j];
                int8_t c = inputs[k];
                
                // f(f(a, b), c)
                uint8_t idx1a = trit_pair_to_index(a, b);
                int8_t temp = TERNARY_DYADIC_GATES[gate_id][idx1a];
                uint8_t idx1b = trit_pair_to_index(temp, c);
                int8_t result1 = TERNARY_DYADIC_GATES[gate_id][idx1b];
                
                // f(a, f(b, c))
                uint8_t idx2a = trit_pair_to_index(b, c);
                temp = TERNARY_DYADIC_GATES[gate_id][idx2a];
                uint8_t idx2b = trit_pair_to_index(a, temp);
                int8_t result2 = TERNARY_DYADIC_GATES[gate_id][idx2b];
                
                if (result1 != result2) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

int8_t ternary_gate_find_identity(uint16_t gate_id)
{
    if (gate_id >= T3_DYADIC_FUNCTION_COUNT) {
        return -2;  // Invalid
    }
    
    // Try each possible identity element: -1, 0, +1
    int8_t candidates[] = {-1, 0, 1};
    
    for (int i = 0; i < 3; i++) {
        int8_t e = candidates[i];
        
        // Check if e is an identity: f(e, x) = x and f(x, e) = x for all x
        bool is_identity = true;
        
        for (int j = 0; j < 3; j++) {
            int8_t x = candidates[j];
            
            // Check f(e, x) = x
            uint8_t idx1 = trit_pair_to_index(e, x);
            if (TERNARY_DYADIC_GATES[gate_id][idx1] != x) {
                is_identity = false;
                break;
            }
            
            // Check f(x, e) = x
            uint8_t idx2 = trit_pair_to_index(x, e);
            if (TERNARY_DYADIC_GATES[gate_id][idx2] != x) {
                is_identity = false;
                break;
            }
        }
        
        if (is_identity) {
            return e;
        }
    }
    
    return -2;  // No identity
}

const char* ternary_gate_get_name(uint16_t gate_id)
{
    // Map well-known functions to names
    switch (gate_id) {
        case 15633: return "KLEENE_AND";
        case 19569: return "KLEENE_OR";
        case 16371: return "CONSENSUS";
        case 3311:  return "MINORITY";
        case 5681:  return "PLUS";
        case 15665: return "TIMES";
        default:    return NULL;
    }
}

void ternary_gate_print_truth_table(uint16_t gate_id)
{
    if (gate_id >= T3_DYADIC_FUNCTION_COUNT) {
        printf("Invalid gate ID: %d\n", gate_id);
        return;
    }
    
    const char* name = ternary_gate_get_name(gate_id);
    if (name) {
        printf("Gate %d (%s):\n", gate_id, name);
    } else {
        printf("Gate %d:\n", gate_id);
    }
    
    printf("     b: ");
    for (int b_val = -1; b_val <= 1; b_val++) {
        printf("%6d", b_val);
    }
    printf("\n");
    
    for (int a_val = -1; a_val <= 1; a_val++) {
        printf("a=%3d: ", a_val);
        for (int b_val = -1; b_val <= 1; b_val++) {
            uint8_t idx = trit_pair_to_index(a_val, b_val);
            printf("%6d", TERNARY_DYADIC_GATES[gate_id][idx]);
        }
        printf("\n");
    }
    
    // Print properties
    bool comm = ternary_gate_is_commutative(gate_id);
    bool assoc = ternary_gate_is_associative(gate_id);
    int8_t ident = ternary_gate_find_identity(gate_id);
    
    printf("Properties: ");
    if (comm) printf("Commutative ");
    if (assoc) printf("Associative ");
    if (ident != -2) printf("Identity=%d", ident);
    printf("\n");
}

