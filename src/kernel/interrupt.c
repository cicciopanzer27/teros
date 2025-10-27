/**
 * @file interrupt.c
 * @brief Interrupt handling implementation
 */

#include "interrupt.h"
#include "console.h"
#include "scheduler.h"
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

// Interrupt nesting support
static uint32_t interrupt_nesting_level = 0;
static bool interrupt_nesting_enabled = false;

// Forward declarations for exception and IRQ handlers
void exception_handler_0(void);
void exception_handler_1(void);
void exception_handler_2(void);
void exception_handler_3(void);
void exception_handler_4(void);
void exception_handler_5(void);
void exception_handler_6(void);
void exception_handler_7(void);
void exception_handler_8(void);
void exception_handler_10(void);
void exception_handler_11(void);
void exception_handler_12(void);
void exception_handler_13(void);
void exception_handler_14(void);
void exception_handler_16(void);
void exception_handler_17(void);
void exception_handler_18(void);
void exception_handler_19(void);
void irq_handler_0(void);
void irq_handler_1(void);
void irq_handler_2(void);
void irq_handler_3(void);
void irq_handler_4(void);
void irq_handler_5(void);
void irq_handler_6(void);
void irq_handler_7(void);
void irq_handler_8(void);
void irq_handler_9(void);
void irq_handler_10(void);
void irq_handler_11(void);
void irq_handler_12(void);
void irq_handler_13(void);
void irq_handler_14(void);
void irq_handler_15(void);

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
    // Track nesting level
    interrupt_nesting_level++;
    
    // Re-enable interrupts if nesting is allowed
    if (interrupt_nesting_enabled && interrupt_nesting_level == 1) {
        asm volatile("sti");
    }
    
    if (interrupt_handlers[interrupt_num] != NULL) {
        interrupt_handlers[interrupt_num](interrupt_num, error_code);
    }
    
    // Disable interrupts before returning
    if (interrupt_nesting_enabled) {
        asm volatile("cli");
    }
    
    interrupt_nesting_level--;
}

void interrupt_send_eoi(uint8_t irq) {
    // Send EOI to slave PIC if IRQ >= 8
    if (irq >= 8) {
        outb(0xA0, 0x20);
    }
    // Always send EOI to master PIC
    outb(0x20, 0x20);
}

void interrupt_enable_nesting(void) {
    interrupt_nesting_enabled = true;
}

void interrupt_disable_nesting(void) {
    interrupt_nesting_enabled = false;
}

uint32_t interrupt_get_nesting_level(void) {
    return interrupt_nesting_level;
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
// PIC (Programmable Interrupt Controller) Management
// =============================================================================

/**
 * @brief Remap PIC IRQs from 0-15 to 32-47 to avoid conflicts with CPU exceptions
 */
static void pic_remap(void) {
    // Save masks
    uint8_t mask1 = inb(0x21);
    uint8_t mask2 = inb(0xA1);
    
    // Start initialization sequence (ICW1)
    outb(0x20, 0x11);  // Master PIC
    outb(0xA0, 0x11);  // Slave PIC
    
    // ICW2: Set vector offsets
    outb(0x21, 0x20);  // Master PIC: IRQs 0-7 → interrupts 32-39
    outb(0xA1, 0x28);  // Slave PIC: IRQs 8-15 → interrupts 40-47
    
    // ICW3: Setup cascade
    outb(0x21, 0x04);  // Master: slave on IRQ2
    outb(0xA1, 0x02);  // Slave: cascade identity
    
    // ICW4: 8086 mode
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    
    // Restore masks
    outb(0x21, mask1);
    outb(0xA1, mask2);
    
    console_puts("PIC: Remapped IRQs 0-15 to interrupts 32-47\n");
}

/**
 * @brief Mask (disable) an IRQ
 */
void pic_mask_irq(uint8_t irq) {
    uint16_t port;
    uint8_t value;
    
    if (irq < 8) {
        port = 0x21;  // Master PIC
    } else {
        port = 0xA1;  // Slave PIC
        irq -= 8;
    }
    
    value = inb(port) | (1 << irq);
    outb(port, value);
}

/**
 * @brief Unmask (enable) an IRQ
 */
void pic_unmask_irq(uint8_t irq) {
    uint16_t port;
    uint8_t value;
    
    if (irq < 8) {
        port = 0x21;  // Master PIC
    } else {
        port = 0xA1;  // Slave PIC
        irq -= 8;
    }
    
    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

// =============================================================================
// INTERRUPT DESCRIPTOR TABLE
// =============================================================================

void idt_init(void) {
    console_puts("IDT: Initializing Interrupt Descriptor Table...\n");

    // Remap PIC before setting up IDT
    pic_remap();

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

// Helper function to print exception with error code
static void exception_panic(const char* message, uint32_t error_code, bool has_error_code) {
    console_puts("\n=== KERNEL PANIC ===\n");
    console_puts(message);
    if (has_error_code) {
        console_puts("\nError Code: 0x");
        char hex[9];
        for (int i = 7; i >= 0; i--) {
            uint8_t digit = (error_code >> (i * 4)) & 0xF;
            hex[7-i] = digit < 10 ? '0' + digit : 'A' + digit - 10;
        }
        hex[8] = '\0';
        console_puts(hex);
    }
    console_puts("\nSystem Halted.\n");
    while(1) { asm volatile("cli; hlt"); }
}

// Exception handlers without error code
void exception_handler_0(void) { exception_panic("EXCEPTION: Divide by zero", 0, false); }
void exception_handler_1(void) { exception_panic("EXCEPTION: Debug", 0, false); }
void exception_handler_2(void) { exception_panic("EXCEPTION: Non-maskable interrupt", 0, false); }
void exception_handler_3(void) { exception_panic("EXCEPTION: Breakpoint", 0, false); }
void exception_handler_4(void) { exception_panic("EXCEPTION: Overflow", 0, false); }
void exception_handler_5(void) { exception_panic("EXCEPTION: Bound range exceeded", 0, false); }
void exception_handler_6(void) { exception_panic("EXCEPTION: Invalid opcode", 0, false); }
void exception_handler_7(void) { exception_panic("EXCEPTION: Device not available", 0, false); }
void exception_handler_16(void) { exception_panic("EXCEPTION: x87 floating point", 0, false); }
void exception_handler_18(void) { exception_panic("EXCEPTION: Machine check", 0, false); }
void exception_handler_19(void) { exception_panic("EXCEPTION: SIMD floating point", 0, false); }

// Exception handlers WITH error code (pushed by CPU)
// Note: In a real implementation, these would be assembly stubs that extract
// the error code from the stack. For now, we mark them as having error codes.
void exception_handler_8(void) { 
    // Double fault - error code is always 0
    exception_panic("EXCEPTION: Double fault", 0, true); 
}

void exception_handler_10(void) { 
    // Invalid TSS - error code contains segment selector
    exception_panic("EXCEPTION: Invalid TSS", 0, true); 
}

void exception_handler_11(void) { 
    // Segment not present - error code contains segment selector
    exception_panic("EXCEPTION: Segment not present", 0, true); 
}

void exception_handler_12(void) { 
    // Stack segment fault - error code contains segment selector
    exception_panic("EXCEPTION: Stack segment fault", 0, true); 
}

void exception_handler_13(void) { 
    // General protection fault - error code contains segment selector
    exception_panic("EXCEPTION: General protection fault", 0, true); 
}

void exception_handler_14(void) { 
    // Page fault - error code describes the fault
    // Bit 0: Present (0 = not present, 1 = protection)
    // Bit 1: Write (0 = read, 1 = write)
    // Bit 2: User (0 = kernel, 1 = user)
    // Bit 3: Reserved write (1 = reserved bit set)
    // Bit 4: Instruction fetch (1 = instruction fetch)
    uint64_t faulting_address;
    asm volatile("mov %%cr2, %0" : "=r"(faulting_address));
    console_puts("\n=== PAGE FAULT ===\n");
    console_puts("Faulting address: 0x");
    char hex[17];
    for (int i = 15; i >= 0; i--) {
        uint8_t digit = (faulting_address >> (i * 4)) & 0xF;
        hex[15-i] = digit < 10 ? '0' + digit : 'A' + digit - 10;
    }
    hex[16] = '\0';
    console_puts(hex);
    exception_panic("\nEXCEPTION: Page fault", 0, true); 
}

void exception_handler_17(void) { 
    // Alignment check - error code is always 0
    exception_panic("EXCEPTION: Alignment check", 0, true); 
}

// =============================================================================
// IRQ HANDLERS
// =============================================================================

// Hardware interrupt handler prototypes
void irq_handler_0(void) {
    // Timer interrupt - call scheduler tick
    scheduler_tick();
    interrupt_send_eoi(IRQ_TIMER);
}

void irq_handler_1(void) {
    // Keyboard interrupt
    uint8_t scancode = inb(0x60);
    // Keyboard scancode handling can be added here
    // keyboard_handle_scancode(scancode);
    (void)scancode; // Suppress unused warning
    interrupt_send_eoi(IRQ_KEYBOARD);
}

void irq_handler_2(void) {
    // Cascade - no action needed
    interrupt_send_eoi(2);
}

void irq_handler_3(void) {
    // COM2 - no action for now
    interrupt_send_eoi(3);
}

void irq_handler_4(void) {
    // COM1 - no action for now
    interrupt_send_eoi(4);
}

void irq_handler_5(void) {
    // LPT2 - no action for now
    interrupt_send_eoi(5);
}

void irq_handler_6(void) {
    // Floppy - no action for now
    interrupt_send_eoi(6);
}

void irq_handler_7(void) {
    // LPT1 - no action for now
    interrupt_send_eoi(7);
}

void irq_handler_8(void) {
    // RTC - no action for now
    interrupt_send_eoi(8);
}

void irq_handler_9(void) {
    // Free - no action
    interrupt_send_eoi(9);
}

void irq_handler_10(void) {
    // Free - no action
    interrupt_send_eoi(10);
}

void irq_handler_11(void) {
    // Free - no action
    interrupt_send_eoi(11);
}

void irq_handler_12(void) {
    // PS/2 mouse - no action for now
    interrupt_send_eoi(12);
}

void irq_handler_13(void) {
    // FPU - no action for now
    interrupt_send_eoi(13);
}

void irq_handler_14(void) {
    // Primary ATA - no action for now
    interrupt_send_eoi(14);
}

void irq_handler_15(void) {
    // Secondary ATA - no action for now
    interrupt_send_eoi(15);
}
