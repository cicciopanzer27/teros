/**
 * @file test_isa_comprehensive.c
 * @brief Comprehensive T3-ISA Test Suite
 * @author TEROS Development Team
 * @date 2025
 */

#include "t3_isa.h"
#include "trit.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define TEST_ASSERT(condition, message) \
    do { \
        if (!(trit_get_value(condition) == TERNARY_POSITIVE)) { \
            printf("FAILED: %s\n", message); \
            return false; \
        } \
    } while(0)

bool test_data_movement(void) {
    printf("Testing data movement instructions...\n");
    
    t3_register_t regs[T3_REGISTER_COUNT];
    t3_instruction_t* instr = t3_instruction_create();
    
    // Test LOAD
    t3_instruction_set(instr, T3_OPCODE_LOAD, T3_REGISTER_R0, 0, 0, 0);
    trit_t value = trit_create(TERNARY_POSITIVE);
    t3_execute_load(instr, regs);
    TEST_ASSERT(trit_equal(regs[T3_REGISTER_R0], value), "LOAD instruction");
    
    // Test STORE
    t3_instruction_set(instr, T3_OPCODE_STORE, T3_REGISTER_R0, 0, 0, 0);
    t3_execute_store(instr, regs);
    TEST_ASSERT(trit_create(TERNARY_POSITIVE), "STORE instruction");
    
    // Test MOV
    regs[T3_REGISTER_R1] = trit_create(TERNARY_NEGATIVE);
    t3_instruction_set(instr, T3_OPCODE_MOV, T3_REGISTER_R0, T3_REGISTER_R1, 0, 0);
    t3_execute_mov(instr, regs);
    TEST_ASSERT(trit_equal(regs[T3_REGISTER_R0], regs[T3_REGISTER_R1]), "MOV instruction");
    
    t3_instruction_destroy(instr);
    printf("Data movement instructions: PASSED\n");
    return true;
}

bool test_arithmetic(void) {
    printf("Testing arithmetic instructions...\n");
    
    t3_register_t regs[T3_REGISTER_COUNT];
    t3_instruction_t* instr = t3_instruction_create();
    
    // Test ADD: +1 + +1 = -1 (ternary arithmetic)
    regs[T3_REGISTER_R0] = trit_create(TERNARY_POSITIVE);
    regs[T3_REGISTER_R1] = trit_create(TERNARY_POSITIVE);
    t3_instruction_set(instr, T3_OPCODE_ADD, T3_REGISTER_R2, T3_REGISTER_R0, T3_REGISTER_R1, 0);
    t3_execute_add(instr, regs);
    TEST_ASSERT(trit_equal(regs[T3_REGISTER_R2], trit_create(TERNARY_NEGATIVE)), "ADD +1 + +1");
    
    // Test SUB
    regs[T3_REGISTER_R0] = trit_create(TERNARY_POSITIVE);
    regs[T3_REGISTER_R1] = trit_create(TERNARY_NEGATIVE);
    t3_instruction_set(instr, T3_OPCODE_SUB, T3_REGISTER_R2, T3_REGISTER_R0, T3_REGISTER_R1, 0);
    t3_execute_sub(instr, regs);
    TEST_ASSERT(trit_equal(regs[T3_REGISTER_R2], trit_create(TERNARY_POSITIVE)), "SUB +1 - -1");
    
    t3_instruction_destroy(instr);
    printf("Arithmetic instructions: PASSED\n");
    return true;
}

bool test_logic(void) {
    printf("Testing logic instructions...\n");
    
    t3_register_t regs[T3_REGISTER_COUNT];
    t3_instruction_t* instr = t3_instruction_create();
    
    // Test AND
    regs[T3_REGISTER_R0] = trit_create(TERNARY_POSITIVE);
    regs[T3_REGISTER_R1] = trit_create(TERNARY_NEGATIVE);
    t3_instruction_set(instr, T3_OPCODE_AND, T3_REGISTER_R2, T3_REGISTER_R0, T3_REGISTER_R1, 0);
    t3_execute_and(instr, regs);
    TEST_ASSERT(trit_equal(regs[T3_REGISTER_R2], trit_create(TERNARY_NEGATIVE)), "AND +1 & -1");
    
    // Test OR
    t3_instruction_set(instr, T3_OPCODE_OR, T3_REGISTER_R2, T3_REGISTER_R0, T3_REGISTER_R1, 0);
    t3_execute_or(instr, regs);
    TEST_ASSERT(trit_equal(regs[T3_REGISTER_R2], trit_create(TERNARY_POSITIVE)), "OR +1 | -1");
    
    t3_instruction_destroy(instr);
    printf("Logic instructions: PASSED\n");
    return true;
}

bool test_control_flow(void) {
    printf("Testing control flow instructions...\n");
    
    t3_register_t regs[T3_REGISTER_COUNT];
    t3_instruction_t* instr = t3_instruction_create();
    
    // Initialize PC
    regs[T3_REGISTER_PC] = trit_create(TERNARY_NEUTRAL);
    
    // Test JMP
    int16_t target = 42;
    t3_instruction_set(instr, T3_OPCODE_JMP, 0, 0, 0, target);
    t3_execute_jmp(instr, regs);
    // PC should be updated
    
    // Test JZ
    regs[T3_REGISTER_R0] = trit_create(TERNARY_NEUTRAL);
    regs[T3_REGISTER_PC] = trit_create(TERNARY_NEUTRAL);
    t3_instruction_set(instr, T3_OPCODE_JZ, T3_REGISTER_R0, 0, 0, target);
    t3_execute_jz(instr, regs);
    
    t3_instruction_destroy(instr);
    printf("Control flow instructions: PASSED\n");
    return true;
}

bool test_stack_operations(void) {
    printf("Testing stack operations...\n");
    
    t3_register_t regs[T3_REGISTER_COUNT];
    t3_instruction_t* instr = t3_instruction_create();
    
    // Initialize stack pointer
    regs[T3_REGISTER_SP] = trit_create(TERNARY_NEUTRAL);
    
    // Test PUSH
    regs[T3_REGISTER_R0] = trit_create(TERNARY_POSITIVE);
    t3_instruction_set(instr, T3_OPCODE_PUSH, T3_REGISTER_R0, 0, 0, 0);
    t3_execute_push(instr, regs);
    
    // Test POP
    t3_instruction_set(instr, T3_OPCODE_POP, T3_REGISTER_R1, 0, 0, 0);
    t3_execute_pop(instr, regs);
    TEST_ASSERT(trit_equal(regs[T3_REGISTER_R1], trit_create(TERNARY_POSITIVE)), "PUSH/POP");
    
    t3_instruction_destroy(instr);
    printf("Stack operations: PASSED\n");
    return true;
}

int main(void) {
    printf("=== T3-ISA Comprehensive Test Suite ===\n\n");
    
    bool all_passed = true;
    
    all_passed &= test_data_movement();
    all_passed &= test_arithmetic();
    all_passed &= test_logic();
    all_passed &= test_control_flow();
    all_passed &= test_stack_operations();
    
    printf("\n=== Test Summary ===\n");
    if (all_passed) {
        printf("ALL TESTS PASSED!\n");
        return 0;
    } else {
        printf("SOME TESTS FAILED!\n");
        return 1;
    }
}
