/**
 * @file boot64.S  
 * @brief TEROS 64-bit Entry Point
 * @author TEROS Development Team
 * 
 * This is the 64-bit portion that:
 * 1. Receives control after Long Mode transition
 * 2. Sets up 64-bit segments and stack
 * 3. Calls kernel_main
 */

.section .text
.code64

.global long_mode_start
.extern kernel_main
.extern multiboot_magic
.extern multiboot_info

long_mode_start:
    /* Setup 64-bit segments */
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    
    /* Setup 64-bit stack (reuse same stack area) */
    movabs $stack_top, %rsp
    xorq %rbp, %rbp
    
    /* Load multiboot parameters (zero-extend 32-bit to 64-bit) */
    movl multiboot_magic, %edi
    movl multiboot_info, %esi
    
    /* Call 64-bit kernel_main */
    call kernel_main
    
    /* Halt if kernel_main returns */
halt64:
    cli
    hlt
    jmp halt64

/* Reference to stack_top from boot32.S */
.extern stack_top

