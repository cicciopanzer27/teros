/**
 * @file linker.ld
 * @brief Linker script for TEROS kernel
 */

/* Entry point */
ENTRY(_start)

SECTIONS
{
    /* Kernel starts at 1MB (standard for x86) */
    . = 0x00100000;
    
    /* Code section */
    .text :
    {
        *(.multiboot)
        *(.text)
    }
    
    /* Read-only data */
    .rodata :
    {
        *(.rodata)
    }
    
    /* Initialized data */
    .data :
    {
        *(.data)
    }
    
    /* Uninitialized data (BSS) */
    .bss :
    {
        *(COMMON)
        *(.bss)
    }
    
    /* End of kernel */
    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }
}

