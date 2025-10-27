/**
 * @file ternary_profiler.h
 * @brief Ternary profiler header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TERNARY_PROFILER_H
#define TERNARY_PROFILER_H

#include "trit.h"
#include "t3_isa.h"
#include "tvm.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <time.h>

// =============================================================================
// TERNARY PROFILER STRUCTURE
// =============================================================================

typedef struct {
    tvm_t* vm;
    size_t* instruction_counts;
    size_t instruction_count_capacity;
    size_t* function_counts;
    size_t function_count_capacity;
    clock_t start_time;
    clock_t end_time;
    bool running;
    bool error;
    char* error_message;
} ternary_profiler_t;

// =============================================================================
// TERNARY PROFILER CREATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Create a new ternary profiler
 * @return A new ternary profiler, or NULL on failure
 */
ternary_profiler_t* ternary_profiler_create(void);

/**
 * @brief Destroy a ternary profiler
 * @param profiler The profiler to destroy
 */
void ternary_profiler_destroy(ternary_profiler_t* profiler);

// =============================================================================
// TERNARY PROFILER VM MANAGEMENT
// =============================================================================

/**
 * @brief Attach a VM to the profiler
 * @param profiler The profiler instance
 * @param vm The VM to attach
 * @return true on success, false on failure
 */
bool ternary_profiler_attach_vm(ternary_profiler_t* profiler, tvm_t* vm);

/**
 * @brief Detach the VM from the profiler
 * @param profiler The profiler instance
 */
void ternary_profiler_detach_vm(ternary_profiler_t* profiler);

/**
 * @brief Get the attached VM
 * @param profiler The profiler instance
 * @return The attached VM, or NULL if none
 */
tvm_t* ternary_profiler_get_vm(ternary_profiler_t* profiler);

// =============================================================================
// TERNARY PROFILER EXECUTION CONTROL
// =============================================================================

/**
 * @brief Start profiling
 * @param profiler The profiler instance
 * @return true on success, false on failure
 */
bool ternary_profiler_start(ternary_profiler_t* profiler);

/**
 * @brief Stop profiling
 * @param profiler The profiler instance
 */
void ternary_profiler_stop(ternary_profiler_t* profiler);

/**
 * @brief Check if profiler is running
 * @param profiler The profiler instance
 * @return true if running, false otherwise
 */
bool ternary_profiler_is_running(ternary_profiler_t* profiler);

// =============================================================================
// TERNARY PROFILER INSTRUCTION PROFILING
// =============================================================================

/**
 * @brief Profile an instruction execution
 * @param profiler The profiler instance
 * @param opcode The instruction opcode
 */
void ternary_profiler_profile_instruction(ternary_profiler_t* profiler, uint8_t opcode);

/**
 * @brief Get instruction execution count
 * @param profiler The profiler instance
 * @param opcode The instruction opcode
 * @return The execution count
 */
size_t ternary_profiler_get_instruction_count(ternary_profiler_t* profiler, uint8_t opcode);

/**
 * @brief Print instruction execution counts
 * @param profiler The profiler instance
 */
void ternary_profiler_print_instruction_counts(ternary_profiler_t* profiler);

// =============================================================================
// TERNARY PROFILER FUNCTION PROFILING
// =============================================================================

/**
 * @brief Profile a function call
 * @param profiler The profiler instance
 * @param function_id The function ID
 */
void ternary_profiler_profile_function(ternary_profiler_t* profiler, int function_id);

/**
 * @brief Get function call count
 * @param profiler The profiler instance
 * @param function_id The function ID
 * @return The call count
 */
size_t ternary_profiler_get_function_count(ternary_profiler_t* profiler, int function_id);

/**
 * @brief Print function call counts
 * @param profiler The profiler instance
 */
void ternary_profiler_print_function_counts(ternary_profiler_t* profiler);

// =============================================================================
// TERNARY PROFILER TIMING
// =============================================================================

/**
 * @brief Get execution time in seconds
 * @param profiler The profiler instance
 * @return The execution time
 */
uint64_t ternary_profiler_get_execution_time(ternary_profiler_t* profiler);

/**
 * @brief Get instructions per second
 * @param profiler The profiler instance
 * @return The instructions per second
 */
uint64_t ternary_profiler_get_instructions_per_second(ternary_profiler_t* profiler);

// =============================================================================
// TERNARY PROFILER STATISTICS
// =============================================================================

/**
 * @brief Get total instruction count
 * @param profiler The profiler instance
 * @return The total instruction count
 */
size_t ternary_profiler_get_total_instructions(ternary_profiler_t* profiler);

/**
 * @brief Get total function call count
 * @param profiler The profiler instance
 * @return The total function call count
 */
size_t ternary_profiler_get_total_functions(ternary_profiler_t* profiler);

/**
 * @brief Get most used instruction
 * @param profiler The profiler instance
 * @return The most used instruction opcode
 */
uint8_t ternary_profiler_get_most_used_instruction(ternary_profiler_t* profiler);

/**
 * @brief Get most used function
 * @param profiler The profiler instance
 * @return The most used function ID
 */
int ternary_profiler_get_most_used_function(ternary_profiler_t* profiler);

// =============================================================================
// TERNARY PROFILER ERROR HANDLING
// =============================================================================

/**
 * @brief Check if profiler has error
 * @param profiler The profiler instance
 * @return true if error, false otherwise
 */
bool ternary_profiler_has_error(ternary_profiler_t* profiler);

/**
 * @brief Get profiler error message
 * @param profiler The profiler instance
 * @return The error message, or NULL if no error
 */
const char* ternary_profiler_get_error_message(ternary_profiler_t* profiler);

/**
 * @brief Set profiler error
 * @param profiler The profiler instance
 * @param message The error message
 */
void ternary_profiler_set_error(ternary_profiler_t* profiler, const char* message);

// =============================================================================
// TERNARY PROFILER UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Print profiling summary
 * @param profiler The profiler instance
 */
void ternary_profiler_print_summary(ternary_profiler_t* profiler);

/**
 * @brief Debug print profiler information
 * @param profiler The profiler instance
 */
void ternary_profiler_debug(ternary_profiler_t* profiler);

#endif // TERNARY_PROFILER_H
