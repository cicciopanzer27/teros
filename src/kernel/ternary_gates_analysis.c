/**
 * @file ternary_gates_analysis.c
 * @brief Ternary Gates Algebraic Properties Analysis
 * @author TEROS Development Team
 * @date 2025
 * 
 * This module analyzes the 19,683 ternary functions to identify:
 * - Commutative functions
 * - Associative functions
 * - Identity elements
 * - Post classification
 * - Functional completeness sets
 */

#include "ternary_gates.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// =============================================================================
// POST CLASSIFICATION
// =============================================================================

/**
 * Post classes for ternary logic (extension of Post's lattice for binary)
 */
typedef enum {
    POST_CLASS_P0   = 1,  // Preserves 0
    POST_CLASS_P1   = 2,  // Preserves 1
    POST_CLASS_P_1  = 4,  // Preserves -1
    POST_CLASS_S    = 8,  // Self-dual
    POST_CLASS_M    = 16, // Monotone
    POST_CLASS_L    = 32, // Linear
} post_class_t;

/**
 * Check if function preserves value 'preserve_value'
 */
static bool preserves_value(uint16_t gate_id, int8_t preserve_value)
{
    // Check f(preserve_value, preserve_value) == preserve_value
    uint8_t idx = (preserve_value + 1) * 3 + (preserve_value + 1);
    return TERNARY_DYADIC_GATES[gate_id][idx] == preserve_value;
}

/**
 * Classify function according to Post's classification
 */
static uint8_t classify_post(uint16_t gate_id)
{
    uint8_t classes = 0;
    
    if (preserves_value(gate_id, 0))  classes |= POST_CLASS_P0;
    if (preserves_value(gate_id, 1))  classes |= POST_CLASS_P1;
    if (preserves_value(gate_id, -1)) classes |= POST_CLASS_P_1;
    
    // TODO: Check for self-dual (S), monotone (M), linear (L)
    
    return classes;
}

// =============================================================================
// STATISTICS STRUCTURE
// =============================================================================

typedef struct {
    uint32_t total_functions;
    uint32_t commutative_count;
    uint32_t associative_count;
    uint32_t has_identity_count;
    uint32_t preserved_0_count;
    uint32_t preserved_1_count;
    uint32_t preserved_neg1_count;
} gate_statistics_t;

// =============================================================================
// ANALYSIS FUNCTIONS
// =============================================================================

/**
 * Count how many functions have each property
 */
gate_statistics_t analyze_all_gates(void)
{
    gate_statistics_t stats = {0};
    stats.total_functions = T3_DYADIC_FUNCTION_COUNT;
    
    printf("Analyzing %d ternary functions...\n", T3_DYADIC_FUNCTION_COUNT);
    
    for (uint32_t i = 0; i < T3_DYADIC_FUNCTION_COUNT; i++) {
        // Check commutativity
        if (ternary_gate_is_commutative(i)) {
            stats.commutative_count++;
        }
        
        // Check associativity
        if (ternary_gate_is_associative(i)) {
            stats.associative_count++;
        }
        
        // Check for identity element
        if (ternary_gate_find_identity(i) != -2) {
            stats.has_identity_count++;
        }
        
        // Check Post classes
        if (preserves_value(i, 0)) stats.preserved_0_count++;
        if (preserves_value(i, 1)) stats.preserved_1_count++;
        if (preserves_value(i, -1)) stats.preserved_neg1_count++;
    }
    
    return stats;
}

/**
 * Print statistics
 */
void print_statistics(const gate_statistics_t* stats)
{
    printf("\n=== TERNARY GATES STATISTICS ===\n");
    printf("Total functions:            %d\n", stats->total_functions);
    printf("Commutative:                %d (%.2f%%)\n", 
           stats->commutative_count,
           100.0 * stats->commutative_count / stats->total_functions);
    printf("Associative:                %d (%.2f%%)\n",
           stats->associative_count,
           100.0 * stats->associative_count / stats->total_functions);
    printf("With identity element:      %d (%.2f%%)\n",
           stats->has_identity_count,
           100.0 * stats->has_identity_count / stats->total_functions);
    printf("Preserve 0:                 %d (%.2f%%)\n",
           stats->preserved_0_count,
           100.0 * stats->preserved_0_count / stats->total_functions);
    printf("Preserve 1:                 %d (%.2f%%)\n",
           stats->preserved_1_count,
           100.0 * stats->preserved_1_count / stats->total_functions);
    printf("Preserve -1:                %d (%.2f%%)\n",
           stats->preserved_neg1_count,
           100.0 * stats->preserved_neg1_count / stats->total_functions);
    printf("================================\n\n");
}

/**
 * Find functionally complete sets
 */
typedef struct {
    uint16_t gates[10];
    uint8_t count;
} complete_set_t;

/**
 * Check if a set of gates is functionally complete
 * (Simplified check - for production, use Post's theorem)
 */
bool is_functionally_complete(const uint16_t* gates, uint8_t count)
{
    // For ternary logic, need at least one gate from each Post class
    // This is a simplified check
    bool has_p0 = false, has_p1 = false, has_p_1 = false;
    
    for (uint8_t i = 0; i < count; i++) {
        uint8_t classes = classify_post(gates[i]);
        if (classes & POST_CLASS_P0) has_p0 = true;
        if (classes & POST_CLASS_P1) has_p1 = true;
        if (classes & POST_CLASS_P_1) has_p_1 = true;
    }
    
    // TODO: Full functional completeness check
    return has_p0 || has_p1 || has_p_1;
}

// =============================================================================
// EXAMPLES OF FUNCTIONALLY COMPLETE SETS
// =============================================================================

void print_functionally_complete_sets(void)
{
    printf("=== FUNCTIONALLY COMPLETE SETS ===\n\n");
    
    // Set 1: Kleene AND, OR, NOT
    uint16_t set1[] = {T3_GATE_KLEENE_AND, T3_GATE_KLEENE_OR, 17}; // NOT is monadic
    printf("Set 1: {KLEENE_AND, KLEENE_OR, NOT}\n");
    printf("  Gates: %d, %d, NOT(id=17)\n", 
           T3_GATE_KLEENE_AND, T3_GATE_KLEENE_OR);
    printf("  Complete: %s\n\n", 
           is_functionally_complete(set1, 2) ? "Yes" : "No");
    
    // Set 2: Consensus, NOT
    uint16_t set2[] = {T3_GATE_CONSENSUS, 17};
    printf("Set 2: {CONSENSUS, NOT}\n");
    printf("  Gates: %d, NOT(id=17)\n", T3_GATE_CONSENSUS);
    printf("  Complete: %s\n\n",
           is_functionally_complete(set2, 1) ? "Yes" : "No");
    
    // Set 3: PLUS, TIMES, NOT
    uint16_t set3[] = {T3_GATE_PLUS, T3_GATE_TIMES, 17};
    printf("Set 3: {PLUS, TIMES, NOT}\n");
    printf("  Gates: %d, %d, NOT(id=17)\n",
           T3_GATE_PLUS, T3_GATE_TIMES);
    printf("  Complete: %s\n\n",
           is_functionally_complete(set3, 2) ? "Yes" : "No");
    
    printf("===================================\n\n");
}

// =============================================================================
// MAIN ANALYSIS FUNCTION
// =============================================================================

void ternary_gates_analyze_all(void)
{
    printf("\n");
    printf("========================================\n");
    printf("TERNARY GATES ALGEBRAIC ANALYSIS\n");
    printf("========================================\n\n");
    
    // Run analysis
    gate_statistics_t stats = analyze_all_gates();
    
    // Print statistics
    print_statistics(&stats);
    
    // Print well-known functions
    printf("Well-known functions:\n");
    ternary_gate_print_truth_table(T3_GATE_KLEENE_AND);
    printf("\n");
    ternary_gate_print_truth_table(T3_GATE_KLEENE_OR);
    printf("\n");
    ternary_gate_print_truth_table(T3_GATE_CONSENSUS);
    printf("\n");
    
    // Print functionally complete sets
    print_functionally_complete_sets();
}

