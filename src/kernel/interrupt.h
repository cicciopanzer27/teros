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

// Interrupt handler type
typedef void (*interrupt_handler_t)(uint32_t interrupt, uint32_t error_code);

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

#endif // INTERRUPT_H

