/**
 * @file boot32.S
 * @brief TEROS 32-bit Bootloader - Multiboot → Long Mode transition
 * @author TEROS Development Team
 * 
 * This is the 32-bit portion that:
 * 1. Receives control from Multiboot
 * 2. Sets up page tables
 * 3. Enables Long Mode
 * 4. Jumps to 64-bit entry point
 */

/* Multiboot header constants */
.set MULTIBOOT_MAGIC,        0x1BADB002
.set MULTIBOOT_FLAGS,        0x00000003   /* ALIGN | MEMINFO */
.set MULTIBOOT_CHECKSUM,     -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

/* CR0/CR4 control bits */
.set CR0_PG,    0x80000000  /* Paging */
.set CR4_PAE,   0x00000020  /* Physical Address Extension */

/* EFER MSR */
.set EFER_MSR,  0xC0000080
.set EFER_LME,  0x00000100  /* Long Mode Enable */

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
 * BSS SECTION
 * ============================================================================= */

.section .bss
.align 4096

/* Page tables */
boot_pml4:
    .skip 4096
boot_pdpt:
    .skip 4096
boot_pd:
    .skip 4096

/* Stack */
.align 16
stack_bottom:
    .skip 16384
.global stack_top
stack_top:

/* =============================================================================
 * DATA SECTION
 * ============================================================================= */

.section .data
.align 4

.global multiboot_magic
.global multiboot_info

multiboot_magic:
    .long 0

multiboot_info:
    .long 0

/* 64-bit GDT */
.align 8
gdt64:
    .quad 0x0000000000000000    /* Null */
    .quad 0x00AF9A000000FFFF    /* Code 64 */
    .quad 0x00AF92000000FFFF    /* Data 64 */

gdt64_pointer:
    .word gdt64_pointer - gdt64 - 1
    .long gdt64

/* =============================================================================
 * TEXT SECTION - 32-bit Code
 * ============================================================================= */

.section .text
.code32

.global _start
.type _start, @function

_start:
    cli
    
    /* Setup stack */
    movl $stack_top, %esp
    movl %esp, %ebp
    
    /* Save multiboot info */
    movl %eax, multiboot_magic
    movl %ebx, multiboot_info
    
    cld
    
    /* Check Long Mode support */
    call check_long_mode
    
    /* Setup page tables */
    call setup_page_tables
    
    /* Enable PAE */
    movl %cr4, %eax
    orl $CR4_PAE, %eax
    movl %eax, %cr4
    
    /* Load PML4 */
    movl $boot_pml4, %eax
    movl %eax, %cr3
    
    /* Enable Long Mode */
    movl $EFER_MSR, %ecx
    rdmsr
    orl $EFER_LME, %eax
    wrmsr
    
    /* Enable paging */
    movl %cr0, %eax
    orl $CR0_PG, %eax
    movl %eax, %cr0
    
    /* Load 64-bit GDT */
    lgdt gdt64_pointer
    
    /* Far jump to 64-bit code */
    ljmp $0x08, $long_mode_start

check_long_mode:
    pushfl
    popl %eax
    movl %eax, %ecx
    xorl $(1 << 21), %eax
    pushl %eax
    popfl
    pushfl
    popl %eax
    pushl %ecx
    popfl
    cmpl %ecx, %eax
    je no_long_mode
    
    movl $0x80000000, %eax
    cpuid
    cmpl $0x80000001, %eax
    jb no_long_mode
    
    movl $0x80000001, %eax
    cpuid
    testl $(1 << 29), %edx
    jz no_long_mode
    
    ret

no_long_mode:
    movl $0xB8000, %edi
    movb $0x4F, %ah
    movb $'N', %al
    movw %ax, (%edi)
    movb $'O', %al
    movw %ax, 2(%edi)
    movb $'6', %al
    movw %ax, 6(%edi)
    movb $'4', %al
    movw %ax, 8(%edi)
    hlt
    jmp .

setup_page_tables:
    /* Clear page tables */
    movl $boot_pml4, %edi
    xorl %eax, %eax
    movl $(4096 * 3 / 4), %ecx
    rep stosl
    
    /* PML4[0] -> PDPT */
    movl $boot_pml4, %edi
    movl $boot_pdpt, %eax
    orl $0x03, %eax
    movl %eax, (%edi)
    
    /* PDPT[0] -> PD */
    movl $boot_pdpt, %edi
    movl $boot_pd, %eax
    orl $0x03, %eax
    movl %eax, (%edi)
    
    /* PD[0] = 2MB page */
    movl $boot_pd, %edi
    movl $0x00000083, %eax
    movl %eax, (%edi)
    
    ret

/* =============================================================================
 * 64-bit Entry Point (defined externally in boot64.S)
 * ============================================================================= */

.extern long_mode_start

