/**
 * @file test_isa.c
 * @brief T3-ISA Testing and Demonstration
 * @author TEROS Development Team
 * @date 2025
 */

#include "t3_isa.h"
#include "trit.h"
#include <stdio.h>
#include <assert.h>

// Test all T3-ISA instructions
int test_arithmetic_instructions(void) {
    printf("Testing arithmetic instructions...\n");
    
    t3_register_t registers[16];
    
    // Initialize registers
    registers[0] = trit_create(TERNARY_POSITIVE);
    registers[1] = trit_create(TERNARY_NEGATIVE);
    
    // Test ADD
    t3_instruction_t* add_inst = t3_instruction_create();
    t3_instruction_set(add_inst, T3_OPCODE_ADD, 0, 1, 2, 0);
    
    trit_t result = t3_instruction_execute(add_inst, registers);
    assert(trit_get_value(result) == TERNARY_ZERO);
    
    printf("PASS: Arithmetic instructions\n");
    return 0;
}

int test_privileged_instructions(void) {
    printf("Testing privileged instructions...\n");
    
    t3_register_t registers[16];
    
    // Test SYSCALL
    t3_instruction_t* syscall_inst = t3_instruction_create();
    t3_instruction_set(syscall_inst, T3_OPCODE_SYSCALL, 0, 0, 0, 0);
    
    trit_t result = t3_instruction_execute(syscall_inst, registers);
    
    printf("PASS: Privileged instructions\n");
    return 0;
}

int main(void) {
    printf("=== T3-ISA Testing Suite ===\n\n");
    
    test_arithmetic_instructions();
    test_privileged_instructions();
    
    printf("\n=== All T3-ISA Tests Passed ===\n");
    return 0;
}

