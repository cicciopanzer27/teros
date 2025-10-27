/**
 * @file kernel_main.c
 * @brief TEROS Kernel main entry point
 * @author TEROS Development Team
 * @date 2025
 */

#include "trit.h"
#include "tvm.h"
#include "console.h"
#include <stdint.h>

// Multiboot structure (simplified)
typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    // ... other fields
} multiboot_info_t;

// Console wrapper functions

void puts(const char* str) {
    console_puts(str);
}

void printf(const char* format, ...) {
    // TODO: Formatted output (for now just print as is)
    puts(format);
}

/**
 * @brief Kernel main entry point
 * @param magic Multiboot magic number
 * @param mbi Multiboot info structure pointer
 */
void kernel_main(uint32_t magic, multiboot_info_t* mbi) {
    // Initialize console
    console_init();
    
    // Print boot message
    puts("\n========================================\n");
    puts("TEROS - Ternary Operating System\n");
    puts("Version 0.1.0 (Pre-Alpha)\n");
    puts("========================================\n\n");
    
    // Verify Multiboot
    if (magic != 0x2BADB002) {
        puts("ERROR: Invalid Multiboot magic number!\n");
        return;
    }
    
    puts("Multiboot verified successfully.\n");
    
    // Print memory information
    if (mbi->flags & 0x01) {
        printf("Available memory: %d KB - %d KB\n", 
               mbi->mem_lower, 
               mbi->mem_upper);
    }
    
    // Approfondiamo trits
    puts("\nTesting Trit operations...\n");
    trit_t t1 = trit_create(TERNARY_POSITIVE);
    trit_t t2 = trit_create(TERNARY_NEGATIVE);
    trit_t result = trit_add(t1, t2);
    
    printf("Trit test: %d + %d = %d\n", 
           trit_get_value(t1),
           trit_get_value(t2),
           trit_get_value(result));
    
    // Initialize TVM (Ternary Virtual Machine)
    puts("\nInitializing Ternary Virtual Machine (TVM)...\n");
    tvm_t* vm = tvm_create(1024);  // 1KB memory
    
    if (vm != NULL) {
        puts("TVM initialized successfully.\n");
        tvm_destroy(vm);
    } else {
        puts("ERROR: Failed to initialize TVM!\n");
    }
    
    // Main kernel loop
    puts("\nKernel initialized. Entering main loop...\n");
    puts("(For now, this is just a hang loop)\n\n");
    
    while (1) {
        // TODO: Process interrupts, run scheduler, etc.
        asm volatile ("hlt");
    }
}

