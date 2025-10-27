/**
 * @file ternary_analyzer.c
 * @brief Ternary code analyzer implementation
 * @author TEROS Development Team
 * @date 2025
 */

#include "ternary_analyzer.h"
#include "trit.h"
#include "t3_isa.h"
#include "ternary_compiler.h"
#include "ternary_optimizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// =============================================================================
// TERNARY ANALYZER IMPLEMENTATION
// =============================================================================

ternary_analyzer_t* ternary_analyzer_create(void) {
    ternary_analyzer_t* analyzer = malloc(sizeof(ternary_analyzer_t));
    if (analyzer == NULL) return NULL;
    
    analyzer->compiler = ternary_compiler_create();
    if (analyzer->compiler == NULL) {
        free(analyzer);
        return NULL;
    }
    
    analyzer->optimizer = ternary_optimizer_create();
    if (analyzer->optimizer == NULL) {
        ternary_compiler_destroy(analyzer->compiler);
        free(analyzer);
        return NULL;
    }
    
    analyzer->error = false;
    analyzer->error_message = NULL;
    
    return analyzer;
}

void ternary_analyzer_destroy(ternary_analyzer_t* analyzer) {
    if (analyzer != NULL) {
        if (analyzer->compiler != NULL) {
            ternary_compiler_destroy(analyzer->compiler);
        }
        if (analyzer->optimizer != NULL) {
            ternary_optimizer_destroy(analyzer->optimizer);
        }
        if (analyzer->error_message != NULL) {
            free(analyzer->error_message);
        }
        free(analyzer);
    }
}

// =============================================================================
// TERNARY ANALYZER CODE ANALYSIS
// =============================================================================

bool ternary_analyzer_analyze_code(ternary_analyzer_t* analyzer, const char* source_code) {
    if (analyzer == NULL || source_code == NULL) {
        return false;
    }
    
    analyzer->error = false;
    
    // Compile source code
    if (!ternary_compiler_generate_code(analyzer->compiler, source_code)) {
        analyzer->error = true;
        analyzer->error_message = strdup("Compilation failed");
        return false;
    }
    
    // Get compiled instructions
    size_t instruction_count = ternary_compiler_get_instruction_count(analyzer->compiler);
    if (instruction_count == 0) {
        analyzer->error = true;
        analyzer->error_message = strdup("No instructions generated");
        return false;
    }
    
    // Analyze instructions
    if (!ternary_analyzer_analyze_instructions(analyzer, instruction_count)) {
        return false;
    }
    
    return true;
}

bool ternary_analyzer_analyze_instructions(ternary_analyzer_t* analyzer, size_t instruction_count) {
    if (analyzer == NULL || instruction_count == 0) {
        return false;
    }
    
    // Analyze instruction patterns
    if (!ternary_analyzer_analyze_patterns(analyzer, instruction_count)) {
        return false;
    }
    
    // Analyze instruction complexity
    if (!ternary_analyzer_analyze_complexity(analyzer, instruction_count)) {
        return false;
    }
    
    // Analyze instruction dependencies
    if (!ternary_analyzer_analyze_dependencies(analyzer, instruction_count)) {
        return false;
    }
    
    return true;
}

// =============================================================================
// TERNARY ANALYZER PATTERN ANALYSIS
// =============================================================================

bool ternary_analyzer_analyze_patterns(ternary_analyzer_t* analyzer, size_t instruction_count) {
    if (analyzer == NULL || instruction_count == 0) {
        return false;
    }
    
    // Count instruction types
    size_t instruction_counts[T3_OPCODE_COUNT] = {0};
    
    for (size_t i = 0; i < instruction_count; i++) {
        t3_instruction_t* instruction = ternary_compiler_get_instruction(analyzer->compiler, i);
        if (instruction != NULL && instruction->valid) {
            instruction_counts[instruction->opcode]++;
        }
    }
    
    // Analyze patterns
    analyzer->most_used_instruction = 0;
    size_t max_count = 0;
    
    for (int i = 0; i < T3_OPCODE_COUNT; i++) {
        if (instruction_counts[i] > max_count) {
            max_count = instruction_counts[i];
            analyzer->most_used_instruction = i;
        }
    }
    
    analyzer->instruction_diversity = 0;
    for (int i = 0; i < T3_OPCODE_COUNT; i++) {
        if (instruction_counts[i] > 0) {
            analyzer->instruction_diversity++;
        }
    }
    
    return true;
}

// =============================================================================
// TERNARY ANALYZER COMPLEXITY ANALYSIS
// =============================================================================

bool ternary_analyzer_analyze_complexity(ternary_analyzer_t* analyzer, size_t instruction_count) {
    if (analyzer == NULL || instruction_count == 0) {
        return false;
    }
    
    analyzer->cyclomatic_complexity = 1;  // Base complexity
    analyzer->instruction_count = instruction_count;
    analyzer->branch_count = 0;
    analyzer->loop_count = 0;
    
    // Analyze complexity
    for (size_t i = 0; i < instruction_count; i++) {
        t3_instruction_t* instruction = ternary_compiler_get_instruction(analyler->compiler, i);
        if (instruction != NULL && instruction->valid) {
            switch (instruction->opcode) {
                case T3_OPCODE_JMP:
                case T3_OPCODE_JZ:
                case T3_OPCODE_JNZ:
                case T3_OPCODE_CALL:
                    analyzer->branch_count++;
                    analyzer->cyclomatic_complexity++;
                    break;
                    
                case T3_OPCODE_RET:
                    analyzer->cyclomatic_complexity++;
                    break;
                    
                default:
                    break;
            }
        }
    }
    
    // Estimate loop count (simplified)
    analyzer->loop_count = analyzer->branch_count / 2;
    
    return true;
}

// =============================================================================
// TERNARY ANALYZER DEPENDENCY ANALYSIS
// =============================================================================

bool ternary_analyzer_analyze_dependencies(ternary_analyzer_t* analyzer, size_t instruction_count) {
    if (analyzer == NULL || instruction_count == 0) {
        return false;
    }
    
    analyzer->dependency_count = 0;
    analyzer->critical_path_length = 0;
    
    // Analyze dependencies between instructions
    for (size_t i = 0; i < instruction_count - 1; i++) {
        t3_instruction_t* current = ternary_compiler_get_instruction(analyzer->compiler, i);
        t3_instruction_t* next = ternary_compiler_get_instruction(analyzer->compiler, i + 1);
        
        if (current != NULL && next != NULL && current->valid && next->valid) {
            if (ternary_analyzer_instructions_dependent(current, next)) {
                analyzer->dependency_count++;
            }
        }
    }
    
    // Estimate critical path length
    analyzer->critical_path_length = instruction_count - analyzer->dependency_count;
    
    return true;
}

bool ternary_analyzer_instructions_dependent(t3_instruction_t* a, t3_instruction_t* b) {
    if (a == NULL || b == NULL) return false;
    
    // Check if instruction A writes to a register that B reads
    int a_writes = ternary_analyzer_get_write_register(a);
    int b_reads = ternary_analyzer_get_read_register(b);
    
    if (a_writes != -1 && b_reads != -1 && a_writes == b_reads) {
        return true;
    }
    
    // Check if instruction B writes to a register that A reads
    int b_writes = ternary_analyzer_get_write_register(b);
    int a_reads = ternary_analyzer_get_read_register(a);
    
    if (b_writes != -1 && a_reads != -1 && b_writes == a_reads) {
        return true;
    }
    
    return false;
}

int ternary_analyzer_get_write_register(t3_instruction_t* instruction) {
    if (instruction == NULL) return -1;
    
    switch (instruction->opcode) {
        case T3_OPCODE_LOAD:
        case T3_OPCODE_ADD:
        case T3_OPCODE_SUB:
        case T3_OPCODE_MUL:
        case T3_OPCODE_DIV:
        case T3_OPCODE_AND:
        case T3_OPCODE_OR:
        case T3_OPCODE_NOT:
        case T3_OPCODE_XOR:
            return instruction->operand1;
        default:
            return -1;
    }
}

int ternary_analyzer_get_read_register(t3_instruction_t* instruction) {
    if (instruction == NULL) return -1;
    
    switch (instruction->opcode) {
        case T3_OPCODE_STORE:
        case T3_OPCODE_ADD:
        case T3_OPCODE_SUB:
        case T3_OPCODE_MUL:
        case T3_OPCODE_DIV:
        case T3_OPCODE_AND:
        case T3_OPCODE_OR:
        case T3_OPCODE_XOR:
        case T3_OPCODE_CMP:
            return instruction->operand2;
        default:
            return -1;
    }
}

// =============================================================================
// TERNARY ANALYZER OPTIMIZATION SUGGESTIONS
// =============================================================================

bool ternary_analyzer_suggest_optimizations(ternary_analyzer_t* analyzer) {
    if (analyzer == NULL) {
        return false;
    }
    
    analyzer->optimization_suggestions = 0;
    
    // Suggest constant folding
    if (analyzer->instruction_count > 10) {
        analyzer->optimization_suggestions |= TERNARY_OPTIMIZATION_CONSTANT_FOLDING;
    }
    
    // Suggest dead code elimination
    if (analyzer->branch_count > 5) {
        analyzer->optimization_suggestions |= TERNARY_OPTIMIZATION_DEAD_CODE_ELIMINATION;
    }
    
    // Suggest loop unrolling
    if (analyzer->loop_count > 2) {
        analyzer->optimization_suggestions |= TERNARY_OPTIMIZATION_LOOP_UNROLLING;
    }
    
    // Suggest instruction scheduling
    if (analyzer->dependency_count > analyzer->instruction_count / 2) {
        analyzer->optimization_suggestions |= TERNARY_OPTIMIZATION_INSTRUCTION_SCHEDULING;
    }
    
    // Suggest register allocation
    if (analyzer->instruction_diversity > 5) {
        analyzer->optimization_suggestions |= TERNARY_OPTIMIZATION_REGISTER_ALLOCATION;
    }
    
    // Suggest peephole optimization
    if (analyzer->instruction_count > 20) {
        analyzer->optimization_suggestions |= TERNARY_OPTIMIZATION_PEEPHOLE;
    }
    
    return true;
}

// =============================================================================
// TERNARY ANALYZER METRICS
// =============================================================================

size_t ternary_analyzer_get_instruction_count(ternary_analyzer_t* analyzer) {
    if (analyzer == NULL) return 0;
    return analyzer->instruction_count;
}

size_t ternary_analyzer_get_branch_count(ternary_analyzer_t* analyzer) {
    if (analyzer == NULL) return 0;
    return analyzer->branch_count;
}

size_t ternary_analyzer_get_loop_count(ternary_analyzer_t* analyzer) {
    if (analyzer == NULL) return 0;
    return analyzer->loop_count;
}

size_t ternary_analyzer_get_dependency_count(ternary_analyzer_t* analyzer) {
    if (analyzer == NULL) return 0;
    return analyzer->dependency_count;
}

size_t ternary_analyzer_get_critical_path_length(ternary_analyzer_t* analyzer) {
    if (analyzer == NULL) return 0;
    return analyzer->critical_path_length;
}

size_t ternary_analyzer_get_cyclomatic_complexity(ternary_analyzer_t* analyzer) {
    if (analyzer == NULL) return 0;
    return analyzer->cyclomatic_complexity;
}

size_t ternary_analyzer_get_instruction_diversity(ternary_analyzer_t* analyzer) {
    if (analyzer == NULL) return 0;
    return analyzer->instruction_diversity;
}

uint8_t ternary_analyzer_get_most_used_instruction(ternary_analyzer_t* analyzer) {
    if (analyzer == NULL) return 0xFF;
    return analyzer->most_used_instruction;
}

uint32_t ternary_analyzer_get_optimization_suggestions(ternary_analyzer_t* analyzer) {
    if (analyzer == NULL) return 0;
    return analyzer->optimization_suggestions;
}

// =============================================================================
// TERNARY ANALYZER ERROR HANDLING
// =============================================================================

bool ternary_analyzer_has_error(ternary_analyzer_t* analyzer) {
    return analyzer != NULL && analyzer->error;
}

const char* ternary_analyzer_get_error_message(ternary_analyzer_t* analyzer) {
    if (analyzer == NULL) return NULL;
    return analyzer->error_message;
}

void ternary_analyzer_set_error(ternary_analyzer_t* analyzer, const char* message) {
    if (analyzer == NULL) return;
    
    analyzer->error = true;
    if (analyzer->error_message != NULL) {
        free(analyzer->error_message);
    }
    analyzer->error_message = message != NULL ? strdup(message) : NULL;
}

// =============================================================================
// TERNARY ANALYZER UTILITY FUNCTIONS
// =============================================================================

void ternary_analyzer_print_analysis(ternary_analyzer_t* analyzer) {
    if (analyzer == NULL) {
        printf("Ternary Analyzer Analysis: NULL\n");
        return;
    }
    
    printf("Ternary Analyzer Analysis:\n");
    printf("  Instruction Count: %zu\n", analyzer->instruction_count);
    printf("  Branch Count: %zu\n", analyzer->branch_count);
    printf("  Loop Count: %zu\n", analyzer->loop_count);
    printf("  Dependency Count: %zu\n", analyzer->dependency_count);
    printf("  Critical Path Length: %zu\n", analyzer->critical_path_length);
    printf("  Cyclomatic Complexity: %zu\n", analyzer->cyclomatic_complexity);
    printf("  Instruction Diversity: %zu\n", analyzer->instruction_diversity);
    printf("  Most Used Instruction: %s\n", t3_opcode_to_string(analyzer->most_used_instruction));
    printf("  Optimization Suggestions: 0x%08x\n", analyzer->optimization_suggestions);
}

void ternary_analyzer_debug(ternary_analyzer_t* analyzer) {
    if (analyzer == NULL) {
        printf("Ternary Analyzer Debug: NULL\n");
        return;
    }
    
    printf("Ternary Analyzer Debug:\n");
    printf("  Error: %s\n", analyzer->error ? "true" : "false");
    printf("  Error Message: %s\n", analyzer->error_message != NULL ? analyzer->error_message : "None");
    printf("  Instruction Count: %zu\n", analyzer->instruction_count);
    printf("  Branch Count: %zu\n", analyzer->branch_count);
    printf("  Loop Count: %zu\n", analyzer->loop_count);
    printf("  Dependency Count: %zu\n", analyzer->dependency_count);
    printf("  Critical Path Length: %zu\n", analyzer->critical_path_length);
    printf("  Cyclomatic Complexity: %zu\n", analyzer->cyclomatic_complexity);
    printf("  Instruction Diversity: %zu\n", analyzer->instruction_diversity);
    printf("  Most Used Instruction: %s\n", t3_opcode_to_string(analyzer->most_used_instruction));
    printf("  Optimization Suggestions: 0x%08x\n", analyzer->optimization_suggestions);
}
