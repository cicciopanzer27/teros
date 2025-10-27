/**
 * @file boot.S
 * @brief Bootloader assembly for TEROS
 * @author TEROS Development Team
 * @date 2025
 * 
 * This bootloader implements a Multiboot-compliant entry point
 * for GRUB and initializes the basic system before jumping to C code.
 */

# Multiboot header
.section .multiboot
.align 4
multiboot_header:
    .long 0x1BADB002              # Magic number
    .long 0x00000003              # Flags (aligned modules + memory info)
    .long -(0x1BADB002 + 0x00000003)  # Checksum

# Stack space
.section .bss
.align 16
stack_bottom:
.skip 16384                       # 16 KB stack
stack_top:

# Entry point
.section .text
.global _start
.type _start, @function
_start:
    # Set up stack pointer
    mov $stack_top, %esp
    
    # Clear direction flag
    cld
    
    # Push Multiboot info
    push %ebx                      # Multiboot info structure
    push %eax                      # Multiboot magic number
    
    # Call kernel_main (C function)
    call kernel_main
    
    # If kernel_main returns, hang
hang:
    cli
    hlt
    jmp hang

.size _start, . - _start

