/**
 * @file test_trit.c
 * @brief Unit tests for Trit (ternary value) implementation
 */

#include "../include/kernel/trit.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

// Test counter
static int tests_run = 0;
static int tests_passed = 0;

// Test assertion macro
#define TEST_ASSERT(condition, message) \
    do { \
        tests_run++; \
        if (condition) { \
            printf("  ✓ %s\n", message); \
            tests_passed++; \
        } else { \
            printf("  ✗ %s\n", message); \
        } \
    } while(0)

// Test Trit creation
void test_trit_creation() {
    printf("Testing Trit creation...\n");
    
    trit_t t1 = trit_create(TERNARY_POSITIVE);
    TEST_ASSERT(trit_get_value(t1) == TERNARY_POSITIVE, "Create positive trit");
    TEST_ASSERT(trit_is_valid(t1), "Positive trit is valid");
    
    trit_t t2 = trit_create(TERNARY_NEUTRAL);
    TEST_ASSERT(trit_get_value(t2) == TERNARY_NEUTRAL, "Create neutral trit");
    TEST_ASSERT(trit_is_valid(t2), "Neutral trit is valid");
    
    trit_t t3 = trit_create(TERNARY_NEGATIVE);
    TEST_ASSERT(trit_get_value(t3) == TERNARY_NEGATIVE, "Create negative trit");
    TEST_ASSERT(trit_is_valid(t3), "Negative trit is valid");
}

// Test Trit logic operations
void test_trit_logic() {
    printf("Testing Trit logic operations...\n");
    
    trit_t t_pos = trit_create(TERNARY_POSITIVE);
    trit_t t_neu = trit_create(TERNARY_NEUTRAL);
    trit_t t_neg = trit_create(TERNARY_NEGATIVE);
    
    // Test AND
    trit_t result = trit_and(t_pos, t_pos);
    TEST_ASSERT(trit_get_value(result) == TERNARY_POSITIVE, "AND: 1 AND 1 = 1");
    
    result = trit_and(t_neg, t_neg);
    TEST_ASSERT(trit_get_value(result) == TERNARY_NEGATIVE, "AND: -1 AND -1 = -1");
    
    result = trit_and(t_pos, t_neg);
    TEST_ASSERT(trit_get_value(result) == TERNARY_NEGATIVE, "AND: 1 AND -1 = -1");
    
    // Test OR
    result = trit_or(t_pos, t_pos);
    TEST_ASSERT(trit_get_value(result) == TERNARY_POSITIVE, "OR: 1 OR 1 = 1");
    
    result = trit_or(t_pos, t_neg);
    TEST_ASSERT(trit_get_value(result) == TERNARY_POSITIVE, "OR: 1 OR -1 = 1");
    
    // Test NOT
    result = trit_not(t_neg);
    TEST_ASSERT(trit_get_value(result) == TERNARY_POSITIVE, "NOT: ~(-1) = 1");

    result = trit_not(t_pos);
    TEST_ASSERT(trit_get_value(result) == TERNARY_NEGATIVE, "NOT: ~(1) = -1");
    
    result = trit_not(t_neu);
    TEST_ASSERT(trit_get_value(result) == TERNARY_NEUTRAL, "NOT: ~(0) = 0");
    
    // Test XOR
    result = trit_xor(t_pos, t_pos);
    TEST_ASSERT(trit_get_value(result) == TERNARY_NEUTRAL, "XOR: 1 XOR 1 = 0");
    
    result = trit_xor(t_pos, t_neg);
    TEST_ASSERT(trit_get_value(result) == TERNARY_POSITIVE, "XOR: 1 XOR -1 = 1");
}

// Test Trit arithmetic operations
void test_trit_arithmetic() {
    printf("Testing Trit arithmetic operations...\n");
    
    trit_t t_pos = trit_create(TERNARY_POSITIVE);
    trit_t t_neu = trit_create(TERNARY_NEUTRAL);
    trit_t t_neg = trit_create(TERNARY_NEGATIVE);
    
    // Test ADD
    trit_t result = trit_add(t_pos, t_neu);
    TEST_ASSERT(trit_get_value(result) == TERNARY_POSITIVE, "ADD: 1 + 0 = 1");
    
    result = trit_add(t_pos, t_neg);
    TEST_ASSERT(trit_get_value(result) == TERNARY_NEUTRAL, "ADD: 1 + (-1) = 0");
    
    result = trit_add(t_neg, t_neg);
    TEST_ASSERT(trit_get_value(result) == TERNARY_NEGATIVE, "ADD: -1 + (-1) = -1");
    
    // Test SUB
    result = trit_subtract(t_pos, t_neu);
    TEST_ASSERT(trit_get_value(result) == TERNARY_POSITIVE, "SUB: 1 - 0 = 1");
    
    result = trit_subtract(t_pos, t_pos);
    TEST_ASSERT(trit_get_value(result) == TERNARY_NEUTRAL, "SUB: 1 - 1 = 0");
    
    // Test MUL
    result = trit_multiply(t_pos, t_pos);
    TEST_ASSERT(trit_get_value(result) == TERNARY_POSITIVE, "MUL: 1 * 1 = 1");
    
    result = trit_multiply(t_neg, t_pos);
    TEST_ASSERT(trit_get_value(result) == TERNARY_NEGATIVE, "MUL: -1 * 1 = -1");
    
    result = trit_multiply(t_pos, t_neu);
    TEST_ASSERT(trit_get_value(result) == TERNARY_NEUTRAL, "MUL: 1 * 0 = 0");
}

int main() {
    printf("========================================\n");
    printf("TEROS Trit Unit Tests\n");
    printf("========================================\n\n");
    
    test_trit_creation();
    printf("\n");
    
    test_trit_logic();
    printf("\n");
    
    test_trit_arithmetic();
    printf("\n");
    
    printf("========================================\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_run - tests_passed);
    printf("========================================\n");
    
    return (tests_run == tests_passed) ? 0 : 1;
}

