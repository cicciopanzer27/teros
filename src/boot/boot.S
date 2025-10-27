/**
 * @file boot.S
 * @brief TEROS Bootloader - Multiboot compliant boot sequence
 * @author TEROS Development Team
 * @date 2025
 */

.section .multiboot
.align 4

# Multiboot header
.long 0x1BADB002              # Magic number
.long 0x00000003              # Flags: align modules, memory map
.long 0xE4524FFB              # Checksum: -(magic + flags)

# Multiboot header end
.long 0                       # Header address
.long 0                       # Load address  
.long 0                       # Load end address
.long 0                       # BSS end address
.long 0                       # Entry address
.long 0                       # Video mode
.long 0                       # Width
.long 0                       # Height
.long 0                       # Depth

.section .text
.global _start
.extern kernel_main
.extern _bss_start
.extern _bss_end
.extern _stack_top

_start:
    # Disable interrupts
    cli
    
    # Save multiboot info
    movl %eax, multiboot_magic
    movl %ebx, multiboot_info
    
    # Setup stack
    movl $_stack_top, %esp
    movl %esp, %ebp
    
    # Clear BSS section
    movl $_bss_start, %edi
    movl $_bss_end, %ecx
    subl %edi, %ecx
    xorl %eax, %eax
    rep stosb
    
    # Setup GDT
    call setup_gdt
    
    # Setup IDT
    call setup_idt
    
    # Enable protected mode features
    call enable_features
    
    # Jump to kernel main
    call kernel_main
    
    # Halt if kernel_main returns
halt_loop:
    hlt
    jmp halt_loop

# =============================================================================
# GDT SETUP
# =============================================================================

.section .data
.align 8

# GDT table
gdt_start:
    .quad 0x0000000000000000    # Null descriptor
    .quad 0x00CF9A000000FFFF    # Code segment (kernel)
    .quad 0x00CF92000000FFFF    # Data segment (kernel)
    .quad 0x00CFFA000000FFFF    # Code segment (user)
    .quad 0x00CFF2000000FFFF    # Data segment (user)
gdt_end:

# GDT descriptor
gdt_descriptor:
    .word gdt_end - gdt_start - 1
    .long gdt_start

# Selectors
.equ KERNEL_CS, 0x08
.equ KERNEL_DS, 0x10
.equ USER_CS, 0x18
.equ USER_DS, 0x20

setup_gdt:
    # Load GDT
    lgdt gdt_descriptor
    
    # Reload segment registers
    movw $KERNEL_DS, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    
    # Far jump to reload CS
    ljmp $KERNEL_CS, $reload_cs
reload_cs:
    ret

# =============================================================================
# IDT SETUP
# =============================================================================

.section .data
.align 8

# IDT table (256 entries)
idt_start:
    .rept 256
        .word 0                # Offset low
        .word KERNEL_CS        # Selector
        .byte 0                # IST
        .byte 0x8E             # Type (interrupt gate)
        .word 0                # Offset mid
        .long 0                # Offset high
        .long 0                # Reserved
    .endr
idt_end:

# IDT descriptor
idt_descriptor:
    .word idt_end - idt_start - 1
    .long idt_start

# Exception handlers
.extern exception_handler_0
.extern exception_handler_1
.extern exception_handler_2
.extern exception_handler_3
.extern exception_handler_4
.extern exception_handler_5
.extern exception_handler_6
.extern exception_handler_7
.extern exception_handler_8
.extern exception_handler_9
.extern exception_handler_10
.extern exception_handler_11
.extern exception_handler_12
.extern exception_handler_13
.extern exception_handler_14
.extern exception_handler_15
.extern exception_handler_16
.extern exception_handler_17
.extern exception_handler_18
.extern exception_handler_19
.extern exception_handler_20
.extern exception_handler_21
.extern exception_handler_22
.extern exception_handler_23
.extern exception_handler_24
.extern exception_handler_25
.extern exception_handler_26
.extern exception_handler_27
.extern exception_handler_28
.extern exception_handler_29
.extern exception_handler_30
.extern exception_handler_31

# IRQ handlers
.extern irq_handler_0
.extern irq_handler_1
.extern irq_handler_2
.extern irq_handler_3
.extern irq_handler_4
.extern irq_handler_5
.extern irq_handler_6
.extern irq_handler_7
.extern irq_handler_8
.extern irq_handler_9
.extern irq_handler_10
.extern irq_handler_11
.extern irq_handler_12
.extern irq_handler_13
.extern irq_handler_14
.extern irq_handler_15

setup_idt:
    # Setup exception handlers (0-31)
    call setup_exception_handlers
    
    # Setup IRQ handlers (32-47)
    call setup_irq_handlers
    
    # Load IDT
    lidt idt_descriptor
    
    ret

setup_exception_handlers:
    # Exception 0: Divide by Zero
    movl $exception_handler_0, %eax
    call set_idt_entry
    
    # Exception 1: Debug
    movl $exception_handler_1, %eax
    call set_idt_entry
    
    # Exception 2: NMI
    movl $exception_handler_2, %eax
    call set_idt_entry
    
    # Exception 3: Breakpoint
    movl $exception_handler_3, %eax
    call set_idt_entry
    
    # Exception 4: Overflow
    movl $exception_handler_4, %eax
    call set_idt_entry
    
    # Exception 5: Bounds Check
    movl $exception_handler_5, %eax
    call set_idt_entry
    
    # Exception 6: Invalid Opcode
    movl $exception_handler_6, %eax
    call set_idt_entry
    
    # Exception 7: Device Not Available
    movl $exception_handler_7, %eax
    call set_idt_entry
    
    # Exception 8: Double Fault
    movl $exception_handler_8, %eax
    call set_idt_entry
    
    # Exception 9: Coprocessor Segment Overrun
    movl $exception_handler_9, %eax
    call set_idt_entry
    
    # Exception 10: Invalid TSS
    movl $exception_handler_10, %eax
    call set_idt_entry
    
    # Exception 11: Segment Not Present
    movl $exception_handler_11, %eax
    call set_idt_entry
    
    # Exception 12: Stack Fault
    movl $exception_handler_12, %eax
    call set_idt_entry
    
    # Exception 13: General Protection Fault
    movl $exception_handler_13, %eax
    call set_idt_entry
    
    # Exception 14: Page Fault
    movl $exception_handler_14, %eax
    call set_idt_entry
    
    # Exception 15: Reserved
    movl $exception_handler_15, %eax
    call set_idt_entry
    
    # Exception 16: x87 FPU Error
    movl $exception_handler_16, %eax
    call set_idt_entry
    
    # Exception 17: Alignment Check
    movl $exception_handler_17, %eax
    call set_idt_entry
    
    # Exception 18: Machine Check
    movl $exception_handler_18, %eax
    call set_idt_entry
    
    # Exception 19: SIMD Floating Point Exception
    movl $exception_handler_19, %eax
    call set_idt_entry
    
    # Exception 20: Virtualization Exception
    movl $exception_handler_20, %eax
    call set_idt_entry
    
    # Exception 21: Control Protection Exception
    movl $exception_handler_21, %eax
    call set_idt_entry
    
    # Exception 22-31: Reserved
    movl $exception_handler_22, %eax
    call set_idt_entry
    movl $exception_handler_23, %eax
    call set_idt_entry
    movl $exception_handler_24, %eax
    call set_idt_entry
    movl $exception_handler_25, %eax
    call set_idt_entry
    movl $exception_handler_26, %eax
    call set_idt_entry
    movl $exception_handler_27, %eax
    call set_idt_entry
    movl $exception_handler_28, %eax
    call set_idt_entry
    movl $exception_handler_29, %eax
    call set_idt_entry
    movl $exception_handler_30, %eax
    call set_idt_entry
    movl $exception_handler_31, %eax
    call set_idt_entry
    
    ret

setup_irq_handlers:
    # IRQ 0: Timer
    movl $irq_handler_0, %eax
    call set_idt_entry
    
    # IRQ 1: Keyboard
    movl $irq_handler_1, %eax
    call set_idt_entry
    
    # IRQ 2: Cascade
    movl $irq_handler_2, %eax
    call set_idt_entry
    
    # IRQ 3: Serial Port 2
    movl $irq_handler_3, %eax
    call set_idt_entry
    
    # IRQ 4: Serial Port 1
    movl $irq_handler_4, %eax
    call set_idt_entry
    
    # IRQ 5: Parallel Port 2
    movl $irq_handler_5, %eax
    call set_idt_entry
    
    # IRQ 6: Floppy Disk
    movl $irq_handler_6, %eax
    call set_idt_entry
    
    # IRQ 7: Parallel Port 1
    movl $irq_handler_7, %eax
    call set_idt_entry
    
    # IRQ 8: RTC
    movl $irq_handler_8, %eax
    call set_idt_entry
    
    # IRQ 9: ACPI
    movl $irq_handler_9, %eax
    call set_idt_entry
    
    # IRQ 10: Reserved
    movl $irq_handler_10, %eax
    call set_idt_entry
    
    # IRQ 11: Reserved
    movl $irq_handler_11, %eax
    call set_idt_entry
    
    # IRQ 12: Mouse
    movl $irq_handler_12, %eax
    call set_idt_entry
    
    # IRQ 13: Coprocessor
    movl $irq_handler_13, %eax
    call set_idt_entry
    
    # IRQ 14: Primary IDE
    movl $irq_handler_14, %eax
    call set_idt_entry
    
    # IRQ 15: Secondary IDE
    movl $irq_handler_15, %eax
    call set_idt_entry
    
    ret

# Set IDT entry helper
# Input: %eax = handler address, %ebx = entry number
set_idt_entry:
    pushl %eax
    pushl %ebx
    
    # Calculate entry offset
    movl %ebx, %ecx
    shll $3, %ecx              # Multiply by 8 (entry size)
    addl $idt_start, %ecx     # Add base address
    
    # Set offset low
    movw %ax, (%ecx)
    
    # Set selector
    movw $KERNEL_CS, 2(%ecx)
    
    # Set type (interrupt gate)
    movb $0x8E, 5(%ecx)
    
    # Set offset high
    shrl $16, %eax
    movw %ax, 6(%ecx)
    
    popl %ebx
    popl %eax
    ret

# =============================================================================
# FEATURE ENABLEMENT
# =============================================================================

enable_features:
    # Enable SSE
    movl %cr4, %eax
    orl $0x200, %eax          # Set OSFXSR bit
    movl %eax, %cr4
    
    # Enable SSE instructions
    movl %cr0, %eax
    andl $0xFFFB, %eax        # Clear EM bit
    orl $0x2, %eax            # Set MP bit
    movl %eax, %cr0
    
    # Initialize SSE
    fninit
    
    ret

# =============================================================================
# DATA SECTION
# =============================================================================

.section .data
.align 4

# Multiboot info storage
multiboot_magic:
    .long 0
multiboot_info:
    .long 0

# Stack
.section .bss
.align 16
_stack_bottom:
    .skip 16384                # 16KB stack
_stack_top:

# BSS markers
.section .bss
.align 4
_bss_start:
_bss_end:
