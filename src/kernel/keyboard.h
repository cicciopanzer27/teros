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
// TERNARY KEY STATES
// =============================================================================

// Ternary key state: -1 (released), 0 (transitioning), +1 (pressed)
typedef int8_t ternary_key_state_t;
#define KEY_RELEASED -1
#define KEY_TRANSITION 0
#define KEY_PRESSED +1

// Extended scancodes
#define SCANCODE_EXT_PREFIX 0xE0
#define SCANCODE_EXT_PREFIX2 0xE1
#define SCANCODE_RIGHT_CTRL 0xE01D
#define SCANCODE_RIGHT_ALT 0xE038
#define SCANCODE_ARROW_UP 0xE048
#define SCANCODE_ARROW_DOWN 0xE050
#define SCANCODE_ARROW_LEFT 0xE04B
#define SCANCODE_ARROW_RIGHT 0xE04D
#define SCANCODE_NUMPAD_ENTER 0xE01C
#define SCANCODE_NUMPAD_SLASH 0xE035

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

/**
 * @brief Get ternary key state
 * @param scan_code Scan code to check
 * @return KEY_RELEASED, KEY_TRANSITION, or KEY_PRESSED
 */
ternary_key_state_t keyboard_get_ternary_state(uint8_t scan_code);

/**
 * @brief Handle extended scan code (E0 prefix)
 * @param scan_code Extended scan code
 */
void keyboard_handle_extended_scan_code(uint8_t scan_code);

/**
 * @brief Get LED state (Caps/Num/Scroll Lock)
 * @param led_mask LED mask
 * @return Current LED state
 */
uint8_t keyboard_get_leds(void);

/**
 * @brief Set LED state
 * @param led_mask LED mask
 */
void keyboard_set_leds(uint8_t led_mask);

#endif // KEYBOARD_H

