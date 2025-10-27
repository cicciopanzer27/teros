/**
 * @file interrupt.h
 * @brief Interrupt handling for TEROS
 */

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>
#include <stdbool.h>

// IRQ numbers
#define IRQ_TIMER     0
#define IRQ_KEYBOARD  1
#define IRQ_SERIAL    3
#define IRQ_MOUSE     12

// Exception types
#define EX_DIVIDE_BY_ZERO    0
#define EX_INVALID_OPCODE    6
#define EX_GENERAL_PROTECTION 13
#define EX_PAGE_FAULT        14

// Privilege levels
#define PRIV_KERNEL   0
#define PRIV_SUPERVISOR 1
#define PRIV_USER     2

// Interrupt Descriptor Table entry
typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} idt_entry_t;

// Interrupt handler type
typedef void (*interrupt_handler_t)(uint32_t interrupt, uint32_t error_code);

// Privilege context
typedef struct {
    uint8_t current_privilege;
    bool interrupts_enabled;
    uint32_t interrupt_enable_stack[8];  // Stack for interrupt enable/disable
    int interrupt_stack_ptr;
} privilege_context_t;

/**
 * @brief Initialize interrupt handling
 */
void interrupt_init(void);

/**
 * @brief Register interrupt handler
 * @param interrupt_num Interrupt number
 * @param handler Handler function
 * @return true on success
 */
bool interrupt_register_handler(uint8_t interrupt_num, interrupt_handler_t handler);

/**
 * @brief Unregister interrupt handler
 * @param interrupt_num Interrupt number
 */
void interrupt_unregister_handler(uint8_t interrupt_num);

/**
 * @brief Enable interrupts
 */
void interrupt_enable(void);

/**
 * @brief Disable interrupts
 */
void interrupt_disable(void);

/**
 * @brief Check if interrupts are enabled
 * @return true if enabled
 */
bool interrupt_enabled(void);

/**
 * @brief Common interrupt handler
 */
void interrupt_handler_common(uint32_t interrupt_num, uint32_t error_code);

/**
 * @brief Send End-Of-Interrupt signal to PIC
 * @param irq IRQ number (0-15)
 */
void interrupt_send_eoi(uint8_t irq);

/**
 * @brief Enable interrupt nesting
 */
void interrupt_enable_nesting(void);

/**
 * @brief Disable interrupt nesting
 */
void interrupt_disable_nesting(void);

/**
 * @brief Get current nesting level
 * @return Nesting level
 */
uint32_t interrupt_get_nesting_level(void);

/**
 * @brief Get current privilege level
 * @return Current privilege level (0=kernel, 1=supervisor, 2=user)
 */
uint8_t privilege_get_current(void);

/**
 * @brief Set privilege level
 * @param level Privilege level to set
 * @return true on success
 */
bool privilege_set(uint8_t level);

/**
 * @brief Check if we can execute privileged instruction
 * @param required_level Required privilege level
 * @return true if allowed
 */
bool privilege_check(uint8_t required_level);

/**
 * @brief Initialize IDT
 */
void idt_init(void);

/**
 * @brief Set IDT entry
 * @param index Entry index
 * @param base Handler address
 * @param privilege Privilege level required
 */
void idt_set_entry(uint8_t index, uint64_t base, uint8_t privilege);

/**
 * @brief Mask (disable) a hardware IRQ
 * @param irq IRQ number (0-15)
 */
void pic_mask_irq(uint8_t irq);

/**
 * @brief Unmask (enable) a hardware IRQ
 * @param irq IRQ number (0-15)
 */
void pic_unmask_irq(uint8_t irq);

#endif // INTERRUPT_H

