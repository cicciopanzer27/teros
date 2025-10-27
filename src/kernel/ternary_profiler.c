/**
 * @file ternary_profiler.c
 * @brief Ternary profiler implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "ternary_profiler.h"
#include "trit.h"
#include "t3_isa.h"
#include "tvm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

// =============================================================================
// TERNARY PROFILER IMPLEMENTATION
// =============================================================================

ternary_profiler_t* ternary_profiler_create(void) {
    ternary_profiler_t* profiler = malloc(sizeof(ternary_profiler_t));
    if (profiler == NULL) return NULL;
    
    profiler->vm = NULL;
    profiler->instruction_counts = NULL;
    profiler->instruction_count_capacity = 0;
    profiler->function_counts = NULL;
    profiler->function_count_capacity = 0;
    profiler->start_time = 0;
    profiler->end_time = 0;
    profiler->running = false;
    profiler->error = false;
    profiler->error_message = NULL;
    
    return profiler;
}

void ternary_profiler_destroy(ternary_profiler_t* profiler) {
    if (profiler != NULL) {
        if (profiler->instruction_counts != NULL) {
            free(profiler->instruction_counts);
        }
        if (profiler->function_counts != NULL) {
            free(profiler->function_counts);
        }
        if (profiler->error_message != NULL) {
            free(profiler->error_message);
        }
        free(profiler);
    }
}

// =============================================================================
// TERNARY PROFILER VM MANAGEMENT
// =============================================================================

bool ternary_profiler_attach_vm(ternary_profiler_t* profiler, tvm_t* vm) {
    if (profiler == NULL || vm == NULL) {
        return false;
    }
    
    profiler->vm = vm;
    return true;
}

void ternary_profiler_detach_vm(ternary_profiler_t* profiler) {
    if (profiler != NULL) {
        profiler->vm = NULL;
    }
}

tvm_t* ternary_profiler_get_vm(ternary_profiler_t* profiler) {
    if (profiler == NULL) return NULL;
    return profiler->vm;
}

// =============================================================================
// TERNARY PROFILER EXECUTION CONTROL
// =============================================================================

bool ternary_profiler_start(ternary_profiler_t* profiler) {
    if (profiler == NULL || profiler->vm == NULL) {
        return false;
    }
    
    profiler->running = true;
    profiler->start_time = clock();
    profiler->end_time = 0;
    profiler->error = false;
    
    // Initialize instruction counts
    if (profiler->instruction_counts == NULL) {
        profiler->instruction_count_capacity = T3_OPCODE_COUNT;
        profiler->instruction_counts = calloc(profiler->instruction_count_capacity, sizeof(size_t));
        if (profiler->instruction_counts == NULL) {
            profiler->error = true;
            profiler->error_message = strdup("Failed to allocate instruction counts");
            return false;
        }
    }
    
    // Initialize function counts
    if (profiler->function_counts == NULL) {
        profiler->function_count_capacity = 16;
        profiler->function_counts = calloc(profiler->function_count_capacity, sizeof(size_t));
        if (profiler->function_counts == NULL) {
            profiler->error = true;
            profiler->error_message = strdup("Failed to allocate function counts");
            return false;
        }
    }
    
    return true;
}

void ternary_profiler_stop(ternary_profiler_t* profiler) {
    if (profiler == NULL) return;
    
    profiler->running = false;
    profiler->end_time = clock();
}

bool ternary_profiler_is_running(ternary_profiler_t* profiler) {
    return profiler != NULL && profiler->running;
}

// =============================================================================
// TERNARY PROFILER INSTRUCTION PROFILING
// =============================================================================

void ternary_profiler_profile_instruction(ternary_profiler_t* profiler, uint8_t opcode) {
    if (profiler == NULL || !profiler->running || opcode >= T3_OPCODE_COUNT) {
        return;
    }
    
    if (profiler->instruction_counts != NULL) {
        profiler->instruction_counts[opcode]++;
    }
}

size_t ternary_profiler_get_instruction_count(ternary_profiler_t* profiler, uint8_t opcode) {
    if (profiler == NULL || opcode >= T3_OPCODE_COUNT) {
        return 0;
    }
    
    if (profiler->instruction_counts != NULL) {
        return profiler->instruction_counts[opcode];
    }
    
    return 0;
}

void ternary_profiler_print_instruction_counts(ternary_profiler_t* profiler) {
    if (profiler == NULL) {
        printf("Ternary Profiler Instruction Counts: NULL\n");
        return;
    }
    
    printf("Ternary Profiler Instruction Counts:\n");
    for (int i = 0; i < T3_OPCODE_COUNT; i++) {
        size_t count = ternary_profiler_get_instruction_count(profiler, i);
        if (count > 0) {
            printf("  %s: %zu\n", t3_opcode_to_string(i), count);
        }
    }
}

// =============================================================================
// TERNARY PROFILER FUNCTION PROFILING
// =============================================================================

void ternary_profiler_profile_function(ternary_profiler_t* profiler, int function_id) {
    if (profiler == NULL || !profiler->running || function_id < 0) {
        return;
    }
    
    if (profiler->function_counts != NULL && function_id < (int)profiler->function_count_capacity) {
        profiler->function_counts[function_id]++;
    }
}

size_t ternary_profiler_get_function_count(ternary_profiler_t* profiler, int function_id) {
    if (profiler == NULL || function_id < 0) {
        return 0;
    }
    
    if (profiler->function_counts != NULL && function_id < (int)profiler->function_count_capacity) {
        return profiler->function_counts[function_id];
    }
    
    return 0;
}

void ternary_profiler_print_function_counts(ternary_profiler_t* profiler) {
    if (profiler == NULL) {
        printf("Ternary Profiler Function Counts: NULL\n");
        return;
    }
    
    printf("Ternary Profiler Function Counts:\n");
    for (size_t i = 0; i < profiler->function_count_capacity; i++) {
        size_t count = profiler->function_counts[i];
        if (count > 0) {
            printf("  Function %zu: %zu\n", i, count);
        }
    }
}

// =============================================================================
// TERNARY PROFILER TIMING
// =============================================================================

double ternary_profiler_get_execution_time(ternary_profiler_t* profiler) {
    if (profiler == NULL) {
        return 0.0;
    }
    
    clock_t end_time = profiler->running ? clock() : profiler->end_time;
    return ((double)(end_time - profiler->start_time)) / CLOCKS_PER_SEC;
}

double ternary_profiler_get_instructions_per_second(ternary_profiler_t* profiler) {
    if (profiler == NULL) {
        return 0.0;
    }
    
    double execution_time = ternary_profiler_get_execution_time(profiler);
    if (execution_time <= 0.0) {
        return 0.0;
    }
    
    size_t total_instructions = 0;
    if (profiler->instruction_counts != NULL) {
        for (int i = 0; i < T3_OPCODE_COUNT; i++) {
            total_instructions += profiler->instruction_counts[i];
        }
    }
    
    return total_instructions / execution_time;
}

// =============================================================================
// TERNARY PROFILER STATISTICS
// =============================================================================

size_t ternary_profiler_get_total_instructions(ternary_profiler_t* profiler) {
    if (profiler == NULL || profiler->instruction_counts == NULL) {
        return 0;
    }
    
    size_t total = 0;
    for (int i = 0; i < T3_OPCODE_COUNT; i++) {
        total += profiler->instruction_counts[i];
    }
    
    return total;
}

size_t ternary_profiler_get_total_functions(ternary_profiler_t* profiler) {
    if (profiler == NULL || profiler->function_counts == NULL) {
        return 0;
    }
    
    size_t total = 0;
    for (size_t i = 0; i < profiler->function_count_capacity; i++) {
        total += profiler->function_counts[i];
    }
    
    return total;
}

uint8_t ternary_profiler_get_most_used_instruction(ternary_profiler_t* profiler) {
    if (profiler == NULL || profiler->instruction_counts == NULL) {
        return 0xFF;
    }
    
    uint8_t most_used = 0;
    size_t max_count = 0;
    
    for (int i = 0; i < T3_OPCODE_COUNT; i++) {
        if (profiler->instruction_counts[i] > max_count) {
            max_count = profiler->instruction_counts[i];
            most_used = i;
        }
    }
    
    return most_used;
}

int ternary_profiler_get_most_used_function(ternary_profiler_t* profiler) {
    if (profiler == NULL || profiler->function_counts == NULL) {
        return -1;
    }
    
    int most_used = -1;
    size_t max_count = 0;
    
    for (size_t i = 0; i < profiler->function_count_capacity; i++) {
        if (profiler->function_counts[i] > max_count) {
            max_count = profiler->function_counts[i];
            most_used = (int)i;
        }
    }
    
    return most_used;
}

// =============================================================================
// TERNARY PROFILER ERROR HANDLING
// =============================================================================

bool ternary_profiler_has_error(ternary_profiler_t* profiler) {
    return profiler != NULL && profiler->error;
}

const char* ternary_profiler_get_error_message(ternary_profiler_t* profiler) {
    if (profiler == NULL) return NULL;
    return profiler->error_message;
}

void ternary_profiler_set_error(ternary_profiler_t* profiler, const char* message) {
    if (profiler == NULL) return;
    
    profiler->error = true;
    if (profiler->error_message != NULL) {
        free(profiler->error_message);
    }
    profiler->error_message = message != NULL ? strdup(message) : NULL;
}

// =============================================================================
// TERNARY PROFILER UTILITY FUNCTIONS
// =============================================================================

void ternary_profiler_print_summary(ternary_profiler_t* profiler) {
    if (profiler == NULL) {
        printf("Ternary Profiler Summary: NULL\n");
        return;
    }
    
    printf("Ternary Profiler Summary:\n");
    printf("  Execution Time: %.6f seconds\n", ternary_profiler_get_execution_time(profiler));
    printf("  Total Instructions: %zu\n", ternary_profiler_get_total_instructions(profiler));
    printf("  Total Functions: %zu\n", ternary_profiler_get_total_functions(profiler));
    printf("  Instructions/Second: %.2f\n", ternary_profiler_get_instructions_per_second(profiler));
    
    uint8_t most_used_instruction = ternary_profiler_get_most_used_instruction(profiler);
    if (most_used_instruction != 0xFF) {
        printf("  Most Used Instruction: %s (%zu times)\n", 
               t3_opcode_to_string(most_used_instruction),
               ternary_profiler_get_instruction_count(profiler, most_used_instruction));
    }
    
    int most_used_function = ternary_profiler_get_most_used_function(profiler);
    if (most_used_function >= 0) {
        printf("  Most Used Function: %d (%zu times)\n", 
               most_used_function,
               ternary_profiler_get_function_count(profiler, most_used_function));
    }
}

void ternary_profiler_debug(ternary_profiler_t* profiler) {
    if (profiler == NULL) {
        printf("Ternary Profiler Debug: NULL\n");
        return;
    }
    
    printf("Ternary Profiler Debug:\n");
    printf("  Running: %s\n", profiler->running ? "true" : "false");
    printf("  Error: %s\n", profiler->error ? "true" : "false");
    printf("  Error Message: %s\n", profiler->error_message != NULL ? profiler->error_message : "None");
    printf("  Start Time: %ld\n", profiler->start_time);
    printf("  End Time: %ld\n", profiler->end_time);
    printf("  Execution Time: %.6f seconds\n", ternary_profiler_get_execution_time(profiler));
    printf("  Instruction Count Capacity: %zu\n", profiler->instruction_count_capacity);
    printf("  Function Count Capacity: %zu\n", profiler->function_count_capacity);
    printf("  Total Instructions: %zu\n", ternary_profiler_get_total_instructions(profiler));
    printf("  Total Functions: %zu\n", ternary_profiler_get_total_functions(profiler));
}
