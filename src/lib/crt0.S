/**
 * @file crt0.S
 * @brief Runtime startup code for TEROS
 * @author TEROS Development Team
 * @date 2025
 */

.section .text.start
.global _start

_start:
    // Initialize stack pointer
    mov %esp, %ebp
    
    // Clear BSS section
    xor %eax, %eax
    mov $_bss_start, %edi
    mov $_bss_end, %ecx
    sub %edi, %ecx
    rep stosb
    
    // Call main function
    call main
    
    // Exit with return code
    push %eax
    call exit

// BSS section markers
.section .bss
_bss_start:
_bss_end:

