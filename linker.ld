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
    .text ALIGN(4K) :
    {
        *(.multiboot)
        *(.text)
        *(.text.*)
    }
    
    /* Read-only data */
    .rodata ALIGN(4K) :
    {
        *(.rodata)
        *(.rodata.*)
    }
    
    /* Initialized data */
    .data ALIGN(4K) :
    {
        *(.data)
        *(.data.*)
    }
    
    /* Uninitialized data (BSS) */
    .bss ALIGN(4K) :
    {
        *(COMMON)
        *(.bss)
        *(.bss.*)
        
        /* BSS markers for boot.S */
        _bss_start = .;
        _bss_end = .;
    }
    
    /* Stack section */
    .stack ALIGN(16) :
    {
        _stack_bottom = .;
        . += 16K;  /* 16KB stack */
        _stack_top = .;
    }
    
    /* End of kernel marker */
    _kernel_end = .;
    
    /* Discard unnecessary sections */
    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note*)
    }
}

