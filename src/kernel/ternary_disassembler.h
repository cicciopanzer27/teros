/**
 * @file ternary_disassembler.h
 * @brief Ternary disassembler header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TERNARY_DISASSEMBLER_H
#define TERNARY_DISASSEMBLER_H

#include "trit.h"
#include "t3_isa.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// =============================================================================
// TERNARY DISASSEMBLER STRUCTURES
// =============================================================================

typedef struct {
    char* name;
    int address;
} ternary_label_t;

typedef struct {
    t3_instruction_t* instructions;
    size_t instruction_count;
    size_t instruction_capacity;
    ternary_label_t* labels;
    size_t label_count;
    size_t label_capacity;
    bool error;
    char* error_message;
} ternary_disassembler_t;

// =============================================================================
// TERNARY DISASSEMBLER CREATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Create a new ternary disassembler
 * @return A new ternary disassembler, or NULL on failure
 */
ternary_disassembler_t* ternary_disassembler_create(void);

/**
 * @brief Destroy a ternary disassembler
 * @param disassembler The disassembler to destroy
 */
void ternary_disassembler_destroy(ternary_disassembler_t* disassembler);

// =============================================================================
// TERNARY DISASSEMBLER INSTRUCTION MANAGEMENT
// =============================================================================

/**
 * @brief Add an instruction to the disassembler
 * @param disassembler The disassembler instance
 * @param instruction The instruction to add
 * @return true on success, false on failure
 */
bool ternary_disassembler_add_instruction(ternary_disassembler_t* disassembler, t3_instruction_t* instruction);

/**
 * @brief Get an instruction by index
 * @param disassembler The disassembler instance
 * @param index The instruction index
 * @return The instruction, or NULL on failure
 */
t3_instruction_t* ternary_disassembler_get_instruction(ternary_disassembler_t* disassembler, size_t index);

/**
 * @brief Get the number of instructions
 * @param disassembler The disassembler instance
 * @return The number of instructions
 */
size_t ternary_disassembler_get_instruction_count(ternary_disassembler_t* disassembler);

// =============================================================================
// TERNARY DISASSEMBLER LABEL MANAGEMENT
// =============================================================================

/**
 * @brief Add a label to the disassembler
 * @param disassembler The disassembler instance
 * @param name The label name
 * @param address The label address
 * @return true on success, false on failure
 */
bool ternary_disassembler_add_label(ternary_disassembler_t* disassembler, const char* name, int address);

/**
 * @brief Find a label by name
 * @param disassembler The disassembler instance
 * @param name The label name
 * @return The label address, or -1 if not found
 */
int ternary_disassembler_find_label(ternary_disassembler_t* disassembler, const char* name);

/**
 * @brief Get a label name by address
 * @param disassembler The disassembler instance
 * @param address The label address
 * @return The label name, or NULL if not found
 */
const char* ternary_disassembler_get_label_name(ternary_disassembler_t* disassembler, int address);

// =============================================================================
// TERNARY DISASSEMBLER DISASSEMBLY
// =============================================================================

/**
 * @brief Disassemble binary data
 * @param disassembler The disassembler instance
 * @param binary_data The binary data to disassemble
 * @param data_size The size of the binary data
 * @return true on success, false on failure
 */
bool ternary_disassembler_disassemble(ternary_disassembler_t* disassembler, const uint8_t* binary_data, size_t data_size);

// =============================================================================
// TERNARY DISASSEMBLER INSTRUCTION FORMATTING
// =============================================================================

/**
 * @brief Format an instruction as a string
 * @param disassembler The disassembler instance
 * @param instruction The instruction to format
 * @return The formatted string (must be freed by caller)
 */
char* ternary_disassembler_format_instruction(ternary_disassembler_t* disassembler, t3_instruction_t* instruction);

/**
 * @brief Format an instruction with labels as a string
 * @param disassembler The disassembler instance
 * @param instruction The instruction to format
 * @param address The instruction address
 * @return The formatted string (must be freed by caller)
 */
char* ternary_disassembler_format_instruction_with_labels(ternary_disassembler_t* disassembler, t3_instruction_t* instruction, int address);

// =============================================================================
// TERNARY DISASSEMBLER SOURCE GENERATION
// =============================================================================

/**
 * @brief Generate source code from disassembled instructions
 * @param disassembler The disassembler instance
 * @return The generated source code (must be freed by caller)
 */
char* ternary_disassembler_generate_source(ternary_disassembler_t* disassembler);

// =============================================================================
// TERNARY DISASSEMBLER ERROR HANDLING
// =============================================================================

/**
 * @brief Check if disassembler has error
 * @param disassembler The disassembler instance
 * @return true if error, false otherwise
 */
bool ternary_disassembler_has_error(ternary_disassembler_t* disassembler);

/**
 * @brief Get disassembler error message
 * @param disassembler The disassembler instance
 * @return The error message, or NULL if no error
 */
const char* ternary_disassembler_get_error_message(ternary_disassembler_t* disassembler);

/**
 * @brief Set disassembler error
 * @param disassembler The disassembler instance
 * @param message The error message
 */
void ternary_disassembler_set_error(ternary_disassembler_t* disassembler, const char* message);

// =============================================================================
// TERNARY DISASSEMBLER UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Print all instructions
 * @param disassembler The disassembler instance
 */
void ternary_disassembler_print_instructions(ternary_disassembler_t* disassembler);

/**
 * @brief Print all labels
 * @param disassembler The disassembler instance
 */
void ternary_disassembler_print_labels(ternary_disassembler_t* disassembler);

/**
 * @brief Debug print disassembler information
 * @param disassembler The disassembler instance
 */
void ternary_disassembler_debug(ternary_disassembler_t* disassembler);

#endif // TERNARY_DISASSEMBLER_H
