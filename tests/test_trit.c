/**
 * @file test_trit.c
 * @brief Tests for Trit Operations
 * @author TEROS Development Team
 * @date 2025
 */

#include <stdio.h>
#include <assert.h>
#include "kernel/trit.h"

void test_trit_basic(void) {
    printf("Testing basic trit operations...\n");
    
    // Create trits
    trit_t t1 = TRIT_FALSE;
    trit_t t2 = TRIT_UNKNOWN;
    trit_t t3 = TRIT_TRUE;
    
    assert(trit_eq(t1, TRIT_FALSE));
    assert(trit_eq(t2, TRIT_UNKNOWN));
    assert(trit_eq(t3, TRIT_TRUE));
    
    printf("PASS: Basic trit operations\n");
}

void test_trit_logic(void) {
    printf("Testing trit logic operations...\n");
    
    trit_t t1 = TRIT_TRUE;
    trit_t t2 = TRIT_FALSE;
    
    trit_t result = trit_and(t1, t2);
    assert(result == TRIT_FALSE);
    
    printf("PASS: Trit logic operations\n");
}

int main(void) {
    printf("=== Trit Tests ===\n");
    
    test_trit_basic();
    test_trit_logic();
    
    printf("\n=== All Tests Passed ===\n");
    return 0;
}

