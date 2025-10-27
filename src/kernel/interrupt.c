/**
 * @file interrupt.c
 * @brief Interrupt handling implementation
 */

#include "interrupt.h"
#include <stddef.h>

#define MAX_INTERRUPTS 256

static interrupt_handler_t interrupt_handlers[MAX_INTERRUPTS];
static bool interrupts_enabled = false;

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
