/**
 * @file ternary_analyzer.h
 * @brief Ternary code analyzer header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TERNARY_ANALYZER_H
#define TERNARY_ANALYZER_H

#include "trit.h"
#include "t3_isa.h"
#include "ternary_compiler.h"
#include "ternary_optimizer.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// =============================================================================
// TERNARY ANALYZER STRUCTURE
// =============================================================================

typedef struct {
    ternary_compiler_t* compiler;
    ternary_optimizer_t* optimizer;
    size_t instruction_count;
    size_t branch_count;
    size_t loop_count;
    size_t dependency_count;
    size_t critical_path_length;
    size_t cyclomatic_complexity;
    size_t instruction_diversity;
    uint8_t most_used_instruction;
    uint32_t optimization_suggestions;
    bool error;
    char* error_message;
} ternary_analyzer_t;

// =============================================================================
// TERNARY ANALYZER CREATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Create a new ternary analyzer
 * @return A new ternary analyzer, or NULL on failure
 */
ternary_analyzer_t* ternary_analyzer_create(void);

/**
 * @brief Destroy a ternary analyzer
 * @param analyzer The analyzer to destroy
 */
void ternary_analyzer_destroy(ternary_analyzer_t* analyzer);

// =============================================================================
// TERNARY ANALYZER CODE ANALYSIS
// =============================================================================

/**
 * @brief Analyze source code
 * @param analyzer The analyzer instance
 * @param source_code The source code to analyze
 * @return true on success, false on failure
 */
bool ternary_analyzer_analyze_code(ternary_analyzer_t* analyzer, const char* source_code);

/**
 * @brief Analyze compiled instructions
 * @param analyzer The analyzer instance
 * @param instruction_count The number of instructions
 * @return true on success, false on failure
 */
bool ternary_analyzer_analyze_instructions(ternary_analyzer_t* analyzer, size_t instruction_count);

// =============================================================================
// TERNARY ANALYZER PATTERN ANALYSIS
// =============================================================================

/**
 * @brief Analyze instruction patterns
 * @param analyzer The analyzer instance
 * @param instruction_count The number of instructions
 * @return true on success, false on failure
 */
bool ternary_analyzer_analyze_patterns(ternary_analyzer_t* analyzer, size_t instruction_count);

// =============================================================================
// TERNARY ANALYZER COMPLEXITY ANALYSIS
// =============================================================================

/**
 * @brief Analyze code complexity
 * @param analyzer The analyzer instance
 * @param instruction_count The number of instructions
 * @return true on success, false on failure
 */
bool ternary_analyzer_analyze_complexity(ternary_analyzer_t* analyzer, size_t instruction_count);

// =============================================================================
// TERNARY ANALYZER DEPENDENCY ANALYSIS
// =============================================================================

/**
 * @brief Analyze instruction dependencies
 * @param analyzer The analyzer instance
 * @param instruction_count The number of instructions
 * @return true on success, false on failure
 */
bool ternary_analyzer_analyze_dependencies(ternary_analyzer_t* analyzer, size_t instruction_count);

/**
 * @brief Check if two instructions are dependent
 * @param a First instruction
 * @param b Second instruction
 * @return true if dependent, false otherwise
 */
bool ternary_analyzer_instructions_dependent(t3_instruction_t* a, t3_instruction_t* b);

/**
 * @brief Get the register written by an instruction
 * @param instruction The instruction
 * @return The register index, or -1 if none
 */
int ternary_analyzer_get_write_register(t3_instruction_t* instruction);

/**
 * @brief Get the register read by an instruction
 * @param instruction The instruction
 * @return The register index, or -1 if none
 */
int ternary_analyzer_get_read_register(t3_instruction_t* instruction);

// =============================================================================
// TERNARY ANALYZER OPTIMIZATION SUGGESTIONS
// =============================================================================

/**
 * @brief Suggest optimizations based on analysis
 * @param analyzer The analyzer instance
 * @return true on success, false on failure
 */
bool ternary_analyzer_suggest_optimizations(ternary_analyzer_t* analyzer);

// =============================================================================
// TERNARY ANALYZER METRICS
// =============================================================================

/**
 * @brief Get instruction count
 * @param analyzer The analyzer instance
 * @return The instruction count
 */
size_t ternary_analyzer_get_instruction_count(ternary_analyzer_t* analyzer);

/**
 * @brief Get branch count
 * @param analyzer The analyzer instance
 * @return The branch count
 */
size_t ternary_analyzer_get_branch_count(ternary_analyzer_t* analyzer);

/**
 * @brief Get loop count
 * @param analyzer The analyzer instance
 * @return The loop count
 */
size_t ternary_analyzer_get_loop_count(ternary_analyzer_t* analyzer);

/**
 * @brief Get dependency count
 * @param analyzer The analyzer instance
 * @return The dependency count
 */
size_t ternary_analyzer_get_dependency_count(ternary_analyzer_t* analyzer);

/**
 * @brief Get critical path length
 * @param analyzer The analyzer instance
 * @return The critical path length
 */
size_t ternary_analyzer_get_critical_path_length(ternary_analyzer_t* analyzer);

/**
 * @brief Get cyclomatic complexity
 * @param analyzer The analyzer instance
 * @return The cyclomatic complexity
 */
size_t ternary_analyzer_get_cyclomatic_complexity(ternary_analyzer_t* analyzer);

/**
 * @brief Get instruction diversity
 * @param analyzer The analyzer instance
 * @return The instruction diversity
 */
size_t ternary_analyzer_get_instruction_diversity(ternary_analyzer_t* analyzer);

/**
 * @brief Get most used instruction
 * @param analyzer The analyzer instance
 * @return The most used instruction opcode
 */
uint8_t ternary_analyzer_get_most_used_instruction(ternary_analyzer_t* analyzer);

/**
 * @brief Get optimization suggestions
 * @param analyzer The analyzer instance
 * @return The optimization suggestions flags
 */
uint32_t ternary_analyzer_get_optimization_suggestions(ternary_analyzer_t* analyzer);

// =============================================================================
// TERNARY ANALYZER ERROR HANDLING
// =============================================================================

/**
 * @brief Check if analyzer has error
 * @param analyzer The analyzer instance
 * @return true if error, false otherwise
 */
bool ternary_analyzer_has_error(ternary_analyzer_t* analyzer);

/**
 * @brief Get analyzer error message
 * @param analyzer The analyzer instance
 * @return The error message, or NULL if no error
 */
const char* ternary_analyzer_get_error_message(ternary_analyzer_t* analyzer);

/**
 * @brief Set analyzer error
 * @param analyzer The analyzer instance
 * @param message The error message
 */
void ternary_analyzer_set_error(ternary_analyzer_t* analyzer, const char* message);

// =============================================================================
// TERNARY ANALYZER UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Print analysis results
 * @param analyzer The analyzer instance
 */
void ternary_analyzer_print_analysis(ternary_analyzer_t* analyzer);

/**
 * @brief Debug print analyzer information
 * @param analyzer The analyzer instance
 */
void ternary_analyzer_debug(ternary_analyzer_t* analyzer);

#endif // TERNARY_ANALYZER_H
