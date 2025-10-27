/**
 * @file context_switch.S
 * @brief Context switch assembly implementation
 * For x86-64 architecture
 */

.section .text
.global context_switch_asm

/**
 * @brief Save current context and switch to new process
 * @param old_pcb Pointer to old PCB's context (in %rdi)
 * @param new_pcb Pointer to new PCB's context (in %rsi)
 * 
 * This is a simplified version - in production would save all registers
 */
context_switch_asm:
    # For now, minimal implementation
    # Just save essential registers (simplified for speed)
    
    # Save callee-saved registers (64-bit)
    pushq %rbp
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    
    # Save current stack pointer to old_pcb
    # %rdi = old_pcb
    movq %rsp, (%rdi)
    
    # Load new stack pointer from new_pcb
    # %rsi = new_pcb
    movq (%rsi), %rsp
    
    # Restore callee-saved registers
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    popq %rbp
    
    ret

