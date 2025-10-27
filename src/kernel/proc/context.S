/**
 * @file context.S
 * @brief Context Switching Assembly Code
 * @author TEROS Development Team
 * @date 2025
 */

.section .text
.global context_switch
.global context_save
.global context_restore

# =============================================================================
# CONTEXT SWITCHING
# =============================================================================

# Context switch between two processes
# Input: %rdi = from process, %rsi = to process
context_switch:
    # Save current process context
    pushq %rbp
    movq %rsp, %rbp
    
    # Save general purpose registers
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    
    # Save segment registers (only %fs and %gs are relevant in x86-64 long mode)
    # NOTE: %ds and %es cannot be pushed in 64-bit mode
    # pushq %ds  # Not supported in 64-bit
    # pushq %es  # Not supported in 64-bit
    # In x86-64, only %fs and %gs are used (for TLS, etc.)
    # For now, we skip saving segment registers as they're not critical for basic context switching
    
    # Save flags
    pushfq
    
    # Save current stack pointer to from process
    movq %rsp, (%rdi)
    
    # Load new stack pointer from to process
    movq (%rsi), %rsp
    
    # Restore flags
    popfq
    
    # Restore segment registers (only %fs and %gs are relevant in x86-64 long mode)
    # NOTE: %ds and %es cannot be popped in 64-bit mode
    # popq %es  # Not supported in 64-bit
    # popq %ds  # Not supported in 64-bit
    # For now, we skip restoring segment registers as they're not critical for basic context switching
    
    # Restore general purpose registers
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdi
    popq %rsi
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax
    
    # Restore base pointer
    popq %rbp
    
    ret

# =============================================================================
# CONTEXT SAVE
# =============================================================================

# Save current context to process structure
# Input: %rdi = process structure
context_save:
    # Save current stack pointer
    movq %rsp, (%rdi)
    
    # Save general purpose registers
    movq %rax, 8(%rdi)
    movq %rbx, 16(%rdi)
    movq %rcx, 24(%rdi)
    movq %rdx, 32(%rdi)
    movq %rsi, 40(%rdi)
    movq %rdi, 48(%rdi)
    movq %r8, 56(%rdi)
    movq %r9, 64(%rdi)
    movq %r10, 72(%rdi)
    movq %r11, 80(%rdi)
    movq %r12, 88(%rdi)
    movq %r13, 96(%rdi)
    movq %r14, 104(%rdi)
    movq %r15, 112(%rdi)
    
    # Save segment registers
    movw %ds, 120(%rdi)
    movw %es, 122(%rdi)
    movw %fs, 124(%rdi)
    movw %gs, 126(%rdi)
    
    # Save flags
    pushfq
    popq %rax
    movq %rax, 128(%rdi)
    
    ret

# =============================================================================
# CONTEXT RESTORE
# =============================================================================

# Restore context from process structure
# Input: %rdi = process structure
context_restore:
    # Restore flags
    movq 128(%rdi), %rax
    pushq %rax
    popfq
    
    # Restore segment registers
    movw 120(%rdi), %ds
    movw 122(%rdi), %es
    movw 124(%rdi), %fs
    movw 126(%rdi), %gs
    
    # Restore general purpose registers
    movq 8(%rdi), %rax
    movq 16(%rdi), %rbx
    movq 24(%rdi), %rcx
    movq 32(%rdi), %rdx
    movq 40(%rdi), %rsi
    movq 48(%rdi), %rdi
    movq 56(%rdi), %r8
    movq 64(%rdi), %r9
    movq 72(%rdi), %r10
    movq 80(%rdi), %r11
    movq 88(%rdi), %r12
    movq 96(%rdi), %r13
    movq 104(%rdi), %r14
    movq 112(%rdi), %r15
    
    # Restore stack pointer
    movq (%rdi), %rsp
    
    ret

# =============================================================================
# TVM CONTEXT SWITCHING
# =============================================================================

# Save TVM context
# Input: %rdi = TVM structure
tvm_context_save:
    # Save TVM registers
    movq %rax, 0(%rdi)      # Register 0
    movq %rbx, 8(%rdi)      # Register 1
    movq %rcx, 16(%rdi)     # Register 2
    movq %rdx, 24(%rdi)     # Register 3
    movq %rsi, 32(%rdi)     # Register 4
    movq %rdi, 40(%rdi)     # Register 5
    movq %r8, 48(%rdi)      # Register 6
    movq %r9, 56(%rdi)      # Register 7
    movq %r10, 64(%rdi)     # Register 8
    movq %r11, 72(%rdi)     # Register 9
    movq %r12, 80(%rdi)     # Register 10
    movq %r13, 88(%rdi)     # Register 11
    movq %r14, 96(%rdi)     # Register 12
    movq %r15, 104(%rdi)    # Register 13
    
    # Save TVM status flags
    pushfq
    popq %rax
    movq %rax, 112(%rdi)    # Status flags
    
    # Save TVM program counter
    movq %rsp, 120(%rdi)    # Program counter
    
    ret

# Restore TVM context
# Input: %rdi = TVM structure
tvm_context_restore:
    # Restore TVM registers
    movq 0(%rdi), %rax      # Register 0
    movq 8(%rdi), %rbx      # Register 1
    movq 16(%rdi), %rcx     # Register 2
    movq 24(%rdi), %rdx     # Register 3
    movq 32(%rdi), %rsi     # Register 4
    movq 40(%rdi), %rdi     # Register 5
    movq 48(%rdi), %r8      # Register 6
    movq 56(%rdi), %r9      # Register 7
    movq 64(%rdi), %r10     # Register 8
    movq 72(%rdi), %r11     # Register 9
    movq 80(%rdi), %r12     # Register 10
    movq 88(%rdi), %r13     # Register 11
    movq 96(%rdi), %r14     # Register 12
    movq 104(%rdi), %r15    # Register 13
    
    # Restore TVM status flags
    movq 112(%rdi), %rax    # Status flags
    pushq %rax
    popfq
    
    # Restore TVM program counter
    movq 120(%rdi), %rsp    # Program counter
    
    ret

# =============================================================================
# INTERRUPT CONTEXT SWITCHING
# =============================================================================

# Save interrupt context
# Input: %rdi = interrupt context structure
interrupt_context_save:
    # Save interrupt stack frame
    movq %rsp, 0(%rdi)      # Stack pointer
    movq %rbp, 8(%rdi)      # Base pointer
    movq %rax, 16(%rdi)     # Return value
    movq %rbx, 24(%rdi)     # General purpose
    movq %rcx, 32(%rdi)     # General purpose
    movq %rdx, 40(%rdi)     # General purpose
    movq %rsi, 48(%rdi)     # Source index
    movq %rdi, 56(%rdi)     # Destination index
    movq %r8, 64(%rdi)      # General purpose
    movq %r9, 72(%rdi)      # General purpose
    movq %r10, 80(%rdi)     # General purpose
    movq %r11, 88(%rdi)     # General purpose
    movq %r12, 96(%rdi)     # General purpose
    movq %r13, 104(%rdi)    # General purpose
    movq %r14, 112(%rdi)    # General purpose
    movq %r15, 120(%rdi)    # General purpose
    
    # Save segment registers
    movw %ds, 128(%rdi)
    movw %es, 130(%rdi)
    movw %fs, 132(%rdi)
    movw %gs, 134(%rdi)
    
    # Save flags
    pushfq
    popq %rax
    movq %rax, 136(%rdi)
    
    ret

# Restore interrupt context
# Input: %rdi = interrupt context structure
interrupt_context_restore:
    # Restore flags
    movq 136(%rdi), %rax
    pushq %rax
    popfq
    
    # Restore segment registers
    movw 128(%rdi), %ds
    movw 130(%rdi), %es
    movw 132(%rdi), %fs
    movw 134(%rdi), %gs
    
    # Restore general purpose registers
    movq 16(%rdi), %rax
    movq 24(%rdi), %rbx
    movq 32(%rdi), %rcx
    movq 40(%rdi), %rdx
    movq 48(%rdi), %rsi
    movq 56(%rdi), %rdi
    movq 64(%rdi), %r8
    movq 72(%rdi), %r9
    movq 80(%rdi), %r10
    movq 88(%rdi), %r11
    movq 96(%rdi), %r12
    movq 104(%rdi), %r13
    movq 112(%rdi), %r14
    movq 120(%rdi), %r15
    
    # Restore stack pointer and base pointer
    movq 8(%rdi), %rbp
    movq 0(%rdi), %rsp
    
    ret

# =============================================================================
# FAST CONTEXT SWITCH
# =============================================================================

# Fast context switch for high-frequency operations
# Input: %rdi = from process, %rsi = to process
fast_context_switch:
    # Save minimal context
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    
    # Save stack pointer
    movq %rsp, (%rdi)
    
    # Load new stack pointer
    movq (%rsi), %rsp
    
    # Restore registers
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax
    
    ret

# =============================================================================
# CONTEXT SWITCH WITH TVM
# =============================================================================

# Context switch including TVM state
# Input: %rdi = from process, %rsi = to process
context_switch_with_tvm:
    # Save current process context
    call context_save
    
    # Save TVM context
    movq 8(%rdi), %rax      # Get TVM pointer
    movq %rax, %rdi
    call tvm_context_save
    
    # Restore new process context
    movq %rsi, %rdi
    call context_restore
    
    # Restore TVM context
    movq 8(%rsi), %rax      # Get TVM pointer
    movq %rax, %rdi
    call tvm_context_restore
    
    ret

