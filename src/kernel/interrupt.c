/**
 * @file interrupt.c
 * @brief Interrupt handling implementation
 */

#include "interrupt.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define MAX_INTERRUPTS 256
#define IDT_SIZE 256

static interrupt_handler_t interrupt_handlers[MAX_INTERRUPTS];
static bool interrupts_enabled = false;
static idt_entry_t idt[IDT_SIZE];
static privilege_context_t priv_ctx = {
    .current_privilege = PRIV_KERNEL,
    .interrupts_enabled = false,
    .interrupt_enable_stack = {0},
    .interrupt_stack_ptr = 0
};

void interrupt_init(void) {
    // Clear all handlers
    for (int i = 0; i < MAX_INTERRUPTS; i++) {
        interrupt_handlers[i] = NULL;
    }
    interrupts_enabled = false;
}

bool interrupt_register_handler(uint8_t interrupt_num, interrupt_handler_t handler) {
    if (interrupt_num >= MAX_INTERRUPTS) {
        return false;
    }
    interrupt_handlers[interrupt_num] = handler;
    return true;
}

void interrupt_unregister_handler(uint8_t interrupt_num) {
    if (interrupt_num < MAX_INTERRUPTS) {
        interrupt_handlers[interrupt_num] = NULL;
    }
}

void interrupt_enable(void) {
    interrupts_enabled = true;
    asm volatile("sti");
}

void interrupt_disable(void) {
    interrupts_enabled = false;
    asm volatile("cli");
}

bool interrupt_enabled(void) {
    return interrupts_enabled;
}

void interrupt_handler_common(uint32_t interrupt_num, uint32_t error_code) {
    if (interrupt_handlers[interrupt_num] != NULL) {
        interrupt_handlers[interrupt_num](interrupt_num, error_code);
    }
    // Unhandled interrupt
}

// =============================================================================
// PRIVILEGE MANAGEMENT
// =============================================================================

uint8_t privilege_get_current(void) {
    return priv_ctx.current_privilege;
}

bool privilege_set(uint8_t level) {
    if (level > PRIV_USER) {
        return false;
    }
    priv_ctx.current_privilege = level;
    return true;
}

bool privilege_check(uint8_t required_level) {
    return priv_ctx.current_privilege <= required_level;
}

// =============================================================================
// INTERRUPT DESCRIPTOR TABLE
// =============================================================================

void idt_init(void) {
    // Clear IDT
    memset(idt, 0, sizeof(idt_entry_t) * IDT_SIZE);
    
    // Setup default exception handlers (simplified)
    // In a real implementation, this would setup proper handlers
}

void idt_set_entry(uint8_t index, uint64_t base, uint8_t privilege) {
    if (index >= IDT_SIZE) {
        return;
    }
    
    idt[index].offset_low = (uint16_t)(base & 0xFFFF);
    idt[index].offset_mid = (uint16_t)((base >> 16) & 0xFFFF);
    idt[index].offset_high = (uint32_t)((base >> 32) & 0xFFFFFFFF);
    
    // Set privilege level in attributes
    idt[index].type_attr = (uint8_t)((privilege << 5) | 0x8E); // Present, DPL, Type
    idt[index].selector = 0x08; // Kernel code segment
}
