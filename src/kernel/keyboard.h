/**
 * @file keyboard.h
 * @brief Keyboard Driver Header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// KEYBOARD FUNCTIONS
// =============================================================================

/**
 * @brief Initialize keyboard
 */
void keyboard_init(void);

/**
 * @brief Handle scan code
 * @param scan_code Scan code
 */
void keyboard_handle_scan_code(uint8_t scan_code);

/**
 * @brief Read key
 * @return ASCII character or 0 if no key available
 */
uint8_t keyboard_read(void);

/**
 * @brief Check if key is available
 * @return true if key is available
 */
bool keyboard_available(void);

/**
 * @brief Register callbacks
 * @param press_callback Callback for key press
 * @param release_callback Callback for key release
 */
void keyboard_register_callbacks(void (*press_callback)(uint8_t, uint8_t), 
                                 void (*release_callback)(uint8_t, uint8_t));

/**
 * @brief Check if keyboard is initialized
 * @return true if initialized
 */
bool keyboard_is_initialized(void);

#endif // KEYBOARD_H

