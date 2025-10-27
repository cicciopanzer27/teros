/**
 * @file boot.S
 * @brief TEROS Bootloader - Multiboot compliant boot sequence
 * @author TEROS Development Team
 * @date 2025
 * 
 * This is a simplified 32-bit bootloader that sets up basic environment
 * and jumps to 64-bit kernel_main
 */

/* Multiboot header constants */
.set MULTIBOOT_MAGIC,        0x1BADB002
.set MULTIBOOT_FLAGS,        0x00000003   /* ALIGN | MEMINFO */
.set MULTIBOOT_CHECKSUM,     -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

/* =============================================================================
 * MULTIBOOT HEADER
 * ============================================================================= */

.section .multiboot
.align 4
multiboot_header:
    .long MULTIBOOT_MAGIC
    .long MULTIBOOT_FLAGS
    .long MULTIBOOT_CHECKSUM

/* =============================================================================
 * BSS SECTION (Stack)
 * ============================================================================= */

.section .bss
.align 16

stack_bottom:
    .skip 16384                 /* 16KB stack */
stack_top:

/* =============================================================================
 * DATA SECTION
 * ============================================================================= */

.section .data
.align 4

/* Multiboot info storage */
.global multiboot_magic
.global multiboot_info

multiboot_magic:
    .long 0

multiboot_info:
    .long 0

/* =============================================================================
 * TEXT SECTION (Boot Code)
 * ============================================================================= */

.section .text
.global _start
.type _start, @function

_start:
    /* Disable interrupts */
    cli
    
    /* Set up stack pointer */
    mov $stack_top, %esp
    mov %esp, %ebp
    
    /* Save multiboot magic and info pointer */
    movl %eax, multiboot_magic
    movl %ebx, multiboot_info
    
    /* Clear direction flag */
    cld
    
    /* Clear BSS section (if needed) */
    /* This is usually done by loader, but we do it to be safe */
    call clear_bss
    
    /* Push multiboot parameters for kernel_main */
    pushl multiboot_info
    pushl multiboot_magic
    
    /* Call kernel main function */
    call kernel_main
    
    /* If kernel_main returns, hang */
    cli
halt:
    hlt
    jmp halt

/* =============================================================================
 * HELPER FUNCTIONS
 * ============================================================================= */

/**
 * clear_bss - Clear BSS section
 * Zeroes out all uninitialized data
 */
clear_bss:
    /* Check if BSS needs clearing */
    movl $_bss_start, %edi
    movl $_bss_end, %ecx
    subl %edi, %ecx
    
    /* If size is 0, skip */
    test %ecx, %ecx
    jz clear_bss_done
    
    /* Clear BSS */
    xorl %eax, %eax
    rep stosb
    
clear_bss_done:
    ret

/* =============================================================================
 * SIZE DIRECTIVE
 * ============================================================================= */

.size _start, . - _start
