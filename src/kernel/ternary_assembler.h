/**
 * @file ternary_assembler.h
 * @brief Ternary assembler header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TERNARY_ASSEMBLER_H
#define TERNARY_ASSEMBLER_H

#include "trit.h"
#include "t3_isa.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// =============================================================================
// TERNARY ASSEMBLER STRUCTURES
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
} ternary_assembler_t;

// =============================================================================
// TERNARY ASSEMBLER CREATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Create a new ternary assembler
 * @return A new ternary assembler, or NULL on failure
 */
ternary_assembler_t* ternary_assembler_create(void);

/**
 * @brief Destroy a ternary assembler
 * @param assembler The assembler to destroy
 */
void ternary_assembler_destroy(ternary_assembler_t* assembler);

// =============================================================================
// TERNARY ASSEMBLER INSTRUCTION MANAGEMENT
// =============================================================================

/**
 * @brief Add an instruction to the assembler
 * @param assembler The assembler instance
 * @param instruction The instruction to add
 * @return true on success, false on failure
 */
bool ternary_assembler_add_instruction(ternary_assembler_t* assembler, t3_instruction_t* instruction);

/**
 * @brief Get an instruction by index
 * @param assembler The assembler instance
 * @param index The instruction index
 * @return The instruction, or NULL on failure
 */
t3_instruction_t* ternary_assembler_get_instruction(ternary_assembler_t* assembler, size_t index);

/**
 * @brief Get the number of instructions
 * @param assembler The assembler instance
 * @return The number of instructions
 */
size_t ternary_assembler_get_instruction_count(ternary_assembler_t* assembler);

// =============================================================================
// TERNARY ASSEMBLER LABEL MANAGEMENT
// =============================================================================

/**
 * @brief Add a label to the assembler
 * @param assembler The assembler instance
 * @param name The label name
 * @param address The label address
 * @return true on success, false on failure
 */
bool ternary_assembler_add_label(ternary_assembler_t* assembler, const char* name, int address);

/**
 * @brief Find a label by name
 * @param assembler The assembler instance
 * @param name The label name
 * @return The label address, or -1 if not found
 */
int ternary_assembler_find_label(ternary_assembler_t* assembler, const char* name);

/**
 * @brief Get a label name by address
 * @param assembler The assembler instance
 * @param address The label address
 * @return The label name, or NULL if not found
 */
const char* ternary_assembler_get_label_name(ternary_assembler_t* assembler, int address);

// =============================================================================
// TERNARY ASSEMBLER PARSING
// =============================================================================

/**
 * @brief Parse a line of assembly code
 * @param assembler The assembler instance
 * @param line The line to parse
 * @return true on success, false on failure
 */
bool ternary_assembler_parse_line(ternary_assembler_t* assembler, const char* line);

/**
 * @brief Parse an instruction from a line
 * @param assembler The assembler instance
 * @param line The line to parse
 * @return true on success, false on failure
 */
bool ternary_assembler_parse_instruction(ternary_assembler_t* assembler, const char* line);

/**
 * @brief Parse an opcode string
 * @param opcode_str The opcode string
 * @return The opcode value, or 0xFF on failure
 */
uint8_t ternary_assembler_parse_opcode(const char* opcode_str);

// =============================================================================
// TERNARY ASSEMBLER CODE GENERATION
// =============================================================================

/**
 * @brief Assemble source code
 * @param assembler The assembler instance
 * @param source_code The source code to assemble
 * @return true on success, false on failure
 */
bool ternary_assembler_assemble(ternary_assembler_t* assembler, const char* source_code);

/**
 * @brief Resolve forward references
 * @param assembler The assembler instance
 * @return true on success, false on failure
 */
bool ternary_assembler_resolve_references(ternary_assembler_t* assembler);

// =============================================================================
// TERNARY ASSEMBLER ERROR HANDLING
// =============================================================================

/**
 * @brief Check if assembler has error
 * @param assembler The assembler instance
 * @return true if error, false otherwise
 */
bool ternary_assembler_has_error(ternary_assembler_t* assembler);

/**
 * @brief Get assembler error message
 * @param assembler The assembler instance
 * @return The error message, or NULL if no error
 */
const char* ternary_assembler_get_error_message(ternary_assembler_t* assembler);

/**
 * @brief Set assembler error
 * @param assembler The assembler instance
 * @param message The error message
 */
void ternary_assembler_set_error(ternary_assembler_t* assembler, const char* message);

// =============================================================================
// TERNARY ASSEMBLER UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Print all instructions
 * @param assembler The assembler instance
 */
void ternary_assembler_print_instructions(ternary_assembler_t* assembler);

/**
 * @brief Print all labels
 * @param assembler The assembler instance
 */
void ternary_assembler_print_labels(ternary_assembler_t* assembler);

/**
 * @brief Debug print assembler information
 * @param assembler The assembler instance
 */
void ternary_assembler_debug(ternary_assembler_t* assembler);

#endif // TERNARY_ASSEMBLER_H
