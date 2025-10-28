/**
 * @file t3_isa.h
 * @brief T3-ISA (Ternary 3-Instruction Set Architecture) header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef T3_ISA_H
#define T3_ISA_H

#include "trit.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// =============================================================================
// T3-ISA CONSTANTS
// =============================================================================

#define T3_REGISTER_COUNT 16
#define T3_OPCODE_COUNT 20

// Register indices
#define T3_REGISTER_R0   0
#define T3_REGISTER_R1   1
#define T3_REGISTER_R2   2
#define T3_REGISTER_R3   3
#define T3_REGISTER_R4   4
#define T3_REGISTER_R5   5
#define T3_REGISTER_R6   6
#define T3_REGISTER_R7   7
#define T3_REGISTER_PC   8   // Program Counter
#define T3_REGISTER_SP   9   // Stack Pointer
#define T3_REGISTER_FP   10  // Frame Pointer
#define T3_REGISTER_LR   11  // Link Register
#define T3_REGISTER_CR   12  // Condition Register
#define T3_REGISTER_ACC  13  // Accumulator
#define T3_REGISTER_TMP  14  // Temporary
#define T3_REGISTER_ZERO 15  // Zero register

// Opcodes
#define T3_OPCODE_LOAD   0
#define T3_OPCODE_STORE  1
#define T3_OPCODE_ADD    2
#define T3_OPCODE_SUB    3
#define T3_OPCODE_MUL    4
#define T3_OPCODE_DIV    5
#define T3_OPCODE_AND    6
#define T3_OPCODE_OR     7
#define T3_OPCODE_NOT    8
#define T3_OPCODE_XOR    9
#define T3_OPCODE_CMP    10
#define T3_OPCODE_JMP    11
#define T3_OPCODE_JZ     12
#define T3_OPCODE_JNZ    13
#define T3_OPCODE_CALL   14
#define T3_OPCODE_RET    15
#define T3_OPCODE_PUSH   16
#define T3_OPCODE_POP    17
#define T3_OPCODE_HALT   18
#define T3_OPCODE_NOP    19
// Extended opcodes
#define T3_OPCODE_SYSCALL 20
#define T3_OPCODE_IRET    21
#define T3_OPCODE_CLI     22
#define T3_OPCODE_STI     23
#define T3_OPCODE_CPUID   24
#define T3_OPCODE_RDTSC   25
#define T3_OPCODE_INT     26
#define T3_OPCODE_MOV     27
#define T3_OPCODE_LEA     28
#define T3_OPCODE_TST     29
#define T3_OPCODE_TGATE   30  // Ternary Gate Operation

// Privilege levels
#define T3_PRIVILEGE_RING0 0  // Kernel mode
#define T3_PRIVILEGE_RING1 1  // Supervisor mode
#define T3_PRIVILEGE_RING2 2  // User mode

// Condition codes
#define T3_CC_EQ  0  // Equal (Zero flag)
#define T3_CC_NE  1  // Not Equal
#define T3_CC_LT  2  // Less Than (Negative flag)
#define T3_CC_LE  3  // Less or Equal
#define T3_CC_GT  4  // Greater Than
#define T3_CC_GE  5  // Greater or Equal

// =============================================================================
// T3-ISA STRUCTURES
// =============================================================================

typedef trit_t t3_register_t;

typedef struct {
    uint8_t opcode;
    uint8_t operand1;
    uint8_t operand2;
    uint8_t operand3;
    int16_t immediate;
    bool valid;
} t3_instruction_t;

// =============================================================================
// T3-ISA INSTRUCTION MANAGEMENT
// =============================================================================

/**
 * @brief Create a new T3 instruction
 * @return A new instruction, or NULL on failure
 */
t3_instruction_t* t3_instruction_create(void);

/**
 * @brief Destroy a T3 instruction
 * @param instruction The instruction to destroy
 */
void t3_instruction_destroy(t3_instruction_t* instruction);

/**
 * @brief Set instruction parameters
 * @param instruction The instruction to set
 * @param opcode The opcode
 * @param operand1 First operand
 * @param operand2 Second operand
 * @param operand3 Third operand
 * @param immediate Immediate value
 */
void t3_instruction_set(t3_instruction_t* instruction, uint8_t opcode, 
                       uint8_t operand1, uint8_t operand2, uint8_t operand3, 
                       int16_t immediate);

/**
 * @brief Check if instruction is valid
 * @param instruction The instruction to check
 * @return true if valid, false otherwise
 */
bool t3_instruction_is_valid(t3_instruction_t* instruction);

// =============================================================================
// T3-ISA INSTRUCTION EXECUTION
// =============================================================================

/**
 * @brief Execute a T3 instruction
 * @param instruction The instruction to execute
 * @param registers The register array
 * @return Result of execution
 */
trit_t t3_instruction_execute(t3_instruction_t* instruction, t3_register_t* registers);

// =============================================================================
// T3-ISA DATA MOVEMENT INSTRUCTIONS
// =============================================================================

trit_t t3_execute_load(t3_instruction_t* instruction, t3_register_t* registers);
trit_t t3_execute_store(t3_instruction_t* instruction, t3_register_t* registers);

// =============================================================================
// T3-ISA ARITHMETIC INSTRUCTIONS
// =============================================================================

trit_t t3_execute_add(t3_instruction_t* instruction, t3_register_t* registers);
trit_t t3_execute_sub(t3_instruction_t* instruction, t3_register_t* registers);
trit_t t3_execute_mul(t3_instruction_t* instruction, t3_register_t* registers);
trit_t t3_execute_div(t3_instruction_t* instruction, t3_register_t* registers);

// =============================================================================
// T3-ISA LOGIC INSTRUCTIONS
// =============================================================================

trit_t t3_execute_and(t3_instruction_t* instruction, t3_register_t* registers);
trit_t t3_execute_or(t3_instruction_t* instruction, t3_register_t* registers);
trit_t t3_execute_not(t3_instruction_t* instruction, t3_register_t* registers);
trit_t t3_execute_xor(t3_instruction_t* instruction, t3_register_t* registers);

// =============================================================================
// T3-ISA COMPARISON INSTRUCTIONS
// =============================================================================

trit_t t3_execute_cmp(t3_instruction_t* instruction, t3_register_t* registers);

// =============================================================================
// T3-ISA CONTROL FLOW INSTRUCTIONS
// =============================================================================

trit_t t3_execute_jmp(t3_instruction_t* instruction, t3_register_t* registers);
trit_t t3_execute_jz(t3_instruction_t* instruction, t3_register_t* registers);
trit_t t3_execute_jnz(t3_instruction_t* instruction, t3_register_t* registers);

// =============================================================================
// T3-ISA STACK INSTRUCTIONS
// =============================================================================

trit_t t3_execute_call(t3_instruction_t* instruction, t3_register_t* registers);
trit_t t3_execute_ret(t3_instruction_t* instruction, t3_register_t* registers);
trit_t t3_execute_push(t3_instruction_t* instruction, t3_register_t* registers);
trit_t t3_execute_pop(t3_instruction_t* instruction, t3_register_t* registers);

// =============================================================================
// T3-ISA SYSTEM INSTRUCTIONS
// =============================================================================

trit_t t3_execute_halt(t3_instruction_t* instruction, t3_register_t* registers);

// =============================================================================
// T3-ISA EXTENDED INSTRUCTIONS
// =============================================================================

trit_t t3_execute_syscall(t3_instruction_t* instruction, t3_register_t* registers);
trit_t t3_execute_iret(t3_instruction_t* instruction, t3_register_t* registers);
trit_t t3_execute_cli(t3_instruction_t* instruction, t3_register_t* registers);
trit_t t3_execute_sti(t3_instruction_t* instruction, t3_register_t* registers);
trit_t t3_execute_cpuid(t3_instruction_t* instruction, t3_register_t* registers);
trit_t t3_execute_rdtsc(t3_instruction_t* instruction, t3_register_t* registers);
trit_t t3_execute_int(t3_instruction_t* instruction, t3_register_t* registers);
trit_t t3_execute_mov(t3_instruction_t* instruction, t3_register_t* registers);
trit_t t3_execute_lea(t3_instruction_t* instruction, t3_register_t* registers);
trit_t t3_execute_tst(t3_instruction_t* instruction, t3_register_t* registers);
trit_t t3_execute_tgate(t3_instruction_t* instruction, t3_register_t* registers);

// =============================================================================
// T3-ISA UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Print instruction information
 * @param instruction The instruction to print
 */
void t3_instruction_print(t3_instruction_t* instruction);

/**
 * @brief Convert opcode to string
 * @param opcode The opcode to convert
 * @return String representation
 */
const char* t3_opcode_to_string(uint8_t opcode);

#endif // T3_ISA_H
