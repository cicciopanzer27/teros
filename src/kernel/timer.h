/**
 * @file timer.h
 * @brief Timer Header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// TIMER FUNCTIONS
// =============================================================================

/**
 * @brief Initialize timer
 */
void timer_init(void);

/**
 * @brief Set timer frequency
 * @param freq Frequency in Hz
 */
void timer_set_frequency(uint32_t freq);

/**
 * @brief Get timer frequency
 * @return Frequency in Hz
 */
uint32_t timer_get_frequency(void);

/**
 * @brief Handle timer interrupt
 */
void timer_handle_interrupt(void);

/**
 * @brief Get timer ticks
 * @return Number of ticks
 */
uint64_t timer_get_ticks(void);

/**
 * @brief Get uptime in milliseconds
 * @return Uptime in ms
 */
uint32_t timer_get_uptime_ms(void);

/**
 * @brief Get uptime in seconds
 * @return Uptime in seconds
 */
uint32_t timer_get_uptime_sec(void);

/**
 * @brief Check if timer is initialized
 * @return true if initialized
 */
bool timer_is_initialized(void);

#endif // TIMER_H

