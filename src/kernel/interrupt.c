/**
 * @file interrupt.c
 * @brief Interrupt handling implementation
 */

#include "interrupt.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// I/O port functions
static inline void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

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
    console_puts("IDT: Initializing Interrupt Descriptor Table...\n");

    // Clear IDT
    memset(idt, 0, sizeof(idt_entry_t) * IDT_SIZE);

    // Setup exception handlers (0-31)
    idt_set_entry(0, (uint64_t)exception_handler_0, 0);   // Divide by zero
    idt_set_entry(1, (uint64_t)exception_handler_1, 0);   // Debug
    idt_set_entry(2, (uint64_t)exception_handler_2, 0);   // Non-maskable interrupt
    idt_set_entry(3, (uint64_t)exception_handler_3, 0);   // Breakpoint
    idt_set_entry(4, (uint64_t)exception_handler_4, 0);   // Overflow
    idt_set_entry(5, (uint64_t)exception_handler_5, 0);   // Bound range exceeded
    idt_set_entry(6, (uint64_t)exception_handler_6, 0);   // Invalid opcode
    idt_set_entry(7, (uint64_t)exception_handler_7, 0);   // Device not available
    idt_set_entry(8, (uint64_t)exception_handler_8, 0);   // Double fault
    idt_set_entry(10, (uint64_t)exception_handler_10, 0);  // Invalid TSS
    idt_set_entry(11, (uint64_t)exception_handler_11, 0);  // Segment not present
    idt_set_entry(12, (uint64_t)exception_handler_12, 0);  // Stack segment fault
    idt_set_entry(13, (uint64_t)exception_handler_13, 0);  // General protection fault
    idt_set_entry(14, (uint64_t)exception_handler_14, 0);  // Page fault
    idt_set_entry(16, (uint64_t)exception_handler_16, 0);  // x87 floating point
    idt_set_entry(17, (uint64_t)exception_handler_17, 0);  // Alignment check
    idt_set_entry(18, (uint64_t)exception_handler_18, 0);  // Machine check
    idt_set_entry(19, (uint64_t)exception_handler_19, 0);  // SIMD floating point

    // Setup hardware interrupt handlers (32-47)
    idt_set_entry(32, (uint64_t)irq_handler_0, 0);   // Timer
    idt_set_entry(33, (uint64_t) irq_handler_1, 0);   // Keyboard
    idt_set_entry(34, (uint64_t) irq_handler_2, 0);   // Cascade
    idt_set_entry(35, (uint64_t) irq_handler_3, 0);   // COM2
    idt_set_entry(36, (uint64_t) irq_handler_4, 0);   // COM1
    idt_set_entry(37, (uint64_t) irq_handler_5, 0);   // LPT2
    idt_set_entry(38, (uint64_t) irq_handler_6, 0);   // Floppy
    idt_set_entry(39, (uint64_t) irq_handler_7, 0);   // LPT1
    idt_set_entry(40, (uint64_t) irq_handler_8, 0);   // RTC
    idt_set_entry(41, (uint64_t) irq_handler_9, 0);   // Free
    idt_set_entry(42, (uint64_t) irq_handler_10, 0);  // Free
    idt_set_entry(43, (uint64_t) irq_handler_11, 0);  // Free
    idt_set_entry(44, (uint64_t) irq_handler_12, 0);  // PS/2 mouse
    idt_set_entry(45, (uint64_t) irq_handler_13, 0);  // FPU
    idt_set_entry(46, (uint64_t) irq_handler_14, 0);  // Primary ATA
    idt_set_entry(47, (uint64_t) irq_handler_15, 0);  // Secondary ATA

    // Load IDT
    idtr_t idtr = {
        .limit = sizeof(idt_entry_t) * IDT_SIZE - 1,
        .base = (uint64_t)&idt
    };

    asm volatile("lidt %0" : : "m"(idtr));

    console_puts("IDT: Initialization complete\n");
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

// =============================================================================
// EXCEPTION HANDLERS
// =============================================================================

// Exception handler prototypes
void exception_handler_0(void) { console_puts("EXCEPTION: Divide by zero\n"); while(1); }
void exception_handler_1(void) { console_puts("EXCEPTION: Debug\n"); while(1); }
void exception_handler_2(void) { console_puts("EXCEPTION: Non-maskable interrupt\n"); while(1); }
void exception_handler_3(void) { console_puts("EXCEPTION: Breakpoint\n"); while(1); }
void exception_handler_4(void) { console_puts("EXCEPTION: Overflow\n"); while(1); }
void exception_handler_5(void) { console_puts("EXCEPTION: Bound range exceeded\n"); while(1); }
void exception_handler_6(void) { console_puts("EXCEPTION: Invalid opcode\n"); while(1); }
void exception_handler_7(void) { console_puts("EXCEPTION: Device not available\n"); while(1); }
void exception_handler_8(void) { console_puts("EXCEPTION: Double fault\n"); while(1); }
void exception_handler_10(void) { console_puts("EXCEPTION: Invalid TSS\n"); while(1); }
void exception_handler_11(void) { console_puts("EXCEPTION: Segment not present\n"); while(1); }
void exception_handler_12(void) { console_puts("EXCEPTION: Stack segment fault\n"); while(1); }
void exception_handler_13(void) { console_puts("EXCEPTION: General protection fault\n"); while(1); }
void exception_handler_14(void) { console_puts("EXCEPTION: Page fault\n"); while(1); }
void exception_handler_16(void) { console_puts("EXCEPTION: x87 floating point\n"); while(1); }
void exception_handler_17(void) { console_puts("EXCEPTION: Alignment check\n"); while(1); }
void exception_handler_18(void) { console_puts("EXCEPTION: Machine check\n"); while(1); }
void exception_handler_19(void) { console_puts("EXCEPTION: SIMD floating point\n"); while(1); }

// =============================================================================
// IRQ HANDLERS
// =============================================================================

// Hardware interrupt handler prototypes
void irq_handler_0(void) {
    // Timer interrupt - call scheduler tick
    scheduler_tick();
    // Send EOI to PIC
    outb(0x20, 0x20);
}

void irq_handler_1(void) {
    // Keyboard interrupt
    uint8_t scancode = inb(0x60);
    // TODO: Implement keyboard scancode handling
    // keyboard_handle_scancode(scancode);
    // Send EOI to PIC
    outb(0x20, 0x20);
}

void irq_handler_2(void) {
    // Cascade - no action needed
    outb(0x20, 0x20);
    outb(0xA0, 0x20);
}

void irq_handler_3(void) {
    // COM2 - no action for now
    outb(0x20, 0x20);
}

void irq_handler_4(void) {
    // COM1 - no action for now
    outb(0x20, 0x20);
}

void irq_handler_5(void) {
    // LPT2 - no action for now
    outb(0x20, 0x20);
}

void irq_handler_6(void) {
    // Floppy - no action for now
    outb(0x20, 0x20);
}

void irq_handler_7(void) {
    // LPT1 - no action for now
    outb(0x20, 0x20);
}

void irq_handler_8(void) {
    // RTC - no action for now
    outb(0x20, 0x20);
    outb(0xA0, 0x20);
}

void irq_handler_9(void) {
    // Free - no action
    outb(0x20, 0x20);
    outb(0xA0, 0x20);
}

void irq_handler_10(void) {
    // Free - no action
    outb(0x20, 0x20);
    outb(0xA0, 0x20);
}

void irq_handler_11(void) {
    // Free - no action
    outb(0x20, 0x20);
    outb(0xA0, 0x20);
}

void irq_handler_12(void) {
    // PS/2 mouse - no action for now
    outb(0x20, 0x20);
    outb(0xA0, 0x20);
}

void irq_handler_13(void) {
    // FPU - no action for now
    outb(0x20, 0x20);
    outb(0xA0, 0x20);
}

void irq_handler_14(void) {
    // Primary ATA - no action for now
    outb(0x20, 0x20);
    outb(0xA0, 0x20);
}

void irq_handler_15(void) {
    // Secondary ATA - no action for now
    outb(0x20, 0x20);
    outb(0xA0, 0x20);
}
