/**
 * @file x86_context.h
 * @brief x86 context structure for process switching
 * This is for x86 hardware, separate from TVM context
 */

#ifndef X86_CONTEXT_H
#define X86_CONTEXT_H

#include <stdint.h>

// x86 register context for context switching
typedef struct {
    // General purpose registers
    uint32_t edi, esi, ebp, esp;
    uint32_t ebx, edx, ecx, eax;
    
    // Segment registers
    uint32_t eip;     // Instruction pointer
    
    // Extended context (if needed)
    uint32_t eflags;  // Flags register
} x86_context_t;

#endif // X86_CONTEXT_H

