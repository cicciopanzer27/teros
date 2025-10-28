/**
 * @file keyboard.c
 * @brief PS/2 Keyboard Driver
 * @author TEROS Development Team
 * @date 2025
 */

#include "keyboard.h"
#include "console.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// =============================================================================
// FORWARD DECLARATIONS
// =============================================================================

static inline void outb(uint16_t port, uint8_t value);
static inline uint8_t inb(uint16_t port);
static inline void memset(void* ptr, int value, size_t size);
uint8_t keyboard_read_config(void);
void keyboard_write_config(uint8_t config);

// =============================================================================
// KEYBOARD IMPLEMENTATION
// =============================================================================

#define KEYBOARD_PORT_DATA 0x60
#define KEYBOARD_PORT_STATUS 0x64
#define KEYBOARD_PORT_COMMAND 0x64

#define KEYBOARD_STATUS_OUTPUT_FULL 0x01
#define KEYBOARD_STATUS_INPUT_FULL 0x02
#define KEYBOARD_STATUS_SYSTEM 0x04
#define KEYBOARD_STATUS_COMMAND 0x08
#define KEYBOARD_STATUS_KEYLOCK 0x10
#define KEYBOARD_STATUS_AUX_OUTPUT_FULL 0x20
#define KEYBOARD_STATUS_TIMEOUT 0x40
#define KEYBOARD_STATUS_PARITY 0x80

#define KEYBOARD_COMMAND_READ_CONFIG 0x20
#define KEYBOARD_COMMAND_WRITE_CONFIG 0x60
#define KEYBOARD_COMMAND_DISABLE_PORT2 0xA7
#define KEYBOARD_COMMAND_ENABLE_PORT2 0xA8
#define KEYBOARD_COMMAND_TEST_PORT2 0xA9
#define KEYBOARD_COMMAND_SELF_TEST 0xAA
#define KEYBOARD_COMMAND_TEST_PORT1 0xAB
#define KEYBOARD_COMMAND_DISABLE_PORT1 0xAD
#define KEYBOARD_COMMAND_ENABLE_PORT1 0xAE

typedef struct {
    bool caps_lock;
    bool num_lock;
    bool scroll_lock;
    bool left_shift;
    bool right_shift;
    bool left_ctrl;
    bool right_ctrl;
    bool left_alt;
    bool right_alt;
    bool initialized;
    uint8_t input_buffer[256];
    uint32_t buffer_head;
    uint32_t buffer_tail;
    uint32_t buffer_count;
    
    // Extended scancode support
    bool extended_prefix;
    uint8_t last_scancode;
    
    // Ternary key states for all keys (256 keys maximum)
    ternary_key_state_t key_states[256];
    
    void (*key_press_callback)(uint8_t scan_code, uint8_t ascii);
    void (*key_release_callback)(uint8_t scan_code, uint8_t ascii);
} keyboard_state_t;

static keyboard_state_t keyboard;

// Scancode to ASCII table (US layout, no shift)
static const uint8_t scancode_to_ascii[] = {
    0,   27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 8,   '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,  'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0
};

// Scancode to ASCII table (US layout, with shift)
static const uint8_t scancode_to_ascii_shift[] = {
    0,   27,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 8,   '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,  'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0
};

// =============================================================================
// KEYBOARD FUNCTIONS
// =============================================================================

void keyboard_init(void) {
    if (keyboard.initialized) {
        return;
    }
    
    console_puts("KEYBOARD: Initializing PS/2 keyboard...\n");
    
    // Initialize keyboard state
    memset(&keyboard, 0, sizeof(keyboard_state_t));
    
    keyboard.buffer_head = 0;
    keyboard.buffer_tail = 0;
    keyboard.buffer_count = 0;
    
    // Clear keyboard buffer
    while (inb(KEYBOARD_PORT_STATUS) & KEYBOARD_STATUS_OUTPUT_FULL) {
        inb(KEYBOARD_PORT_DATA);
    }
    
    // Enable keyboard interrupts
    uint8_t config = keyboard_read_config();
    config |= 0x01; // Enable keyboard interrupt
    keyboard_write_config(config);
    
    keyboard.initialized = true;
    console_puts("KEYBOARD: PS/2 keyboard initialized\n");
}

uint8_t keyboard_read_config(void) {
    outb(KEYBOARD_PORT_COMMAND, KEYBOARD_COMMAND_READ_CONFIG);
    uint8_t config = inb(KEYBOARD_PORT_DATA);
    return config;
}

void keyboard_write_config(uint8_t config) {
    outb(KEYBOARD_PORT_COMMAND, KEYBOARD_COMMAND_WRITE_CONFIG);
    outb(KEYBOARD_PORT_DATA, config);
}

void keyboard_handle_scan_code(uint8_t scan_code) {
    if (!keyboard.initialized) {
        return;
    }
    
    // Handle extended scancode prefix (E0)
    if (scan_code == SCANCODE_EXT_PREFIX) {
        keyboard.extended_prefix = true;
        return;
    }
    
    // Handle extended scancodes (with E0 prefix)
    if (keyboard.extended_prefix) {
        keyboard_handle_extended_scan_code(scan_code);
        keyboard.extended_prefix = false;
        return;
    }
    
    bool make = true;
    
    // Check if this is a break code
    if (scan_code & 0x80) {
        make = false;
        scan_code &= 0x7F;
    }
    
    // Update ternary key state: -1 (released), 0 (transitioning), +1 (pressed)
    ternary_key_state_t prev_state = keyboard.key_states[scan_code];
    ternary_key_state_t new_state = make ? KEY_PRESSED : KEY_RELEASED;
    
    // Ternary state transition using gates:
    // - if prev != new: set to KEY_TRANSITION (0) briefly
    // - then set to final state (new)
    if (prev_state != new_state) {
        keyboard.key_states[scan_code] = KEY_TRANSITION;  // Transition state
        // Note: In real implementation, would delay before final state
        keyboard.key_states[scan_code] = new_state;       // Final state
    }
    
    keyboard.last_scancode = scan_code;
    
    // Handle special keys
    switch (scan_code) {
        case 0x2A: // Left shift
            keyboard.left_shift = make;
            break;
        case 0x36: // Right shift
            keyboard.right_shift = make;
            break;
        case 0x1D: // Left control
            keyboard.left_ctrl = make;
            break;
        case 0x1D | 0x80: // Right control
            keyboard.right_ctrl = make;
            break;
        case 0x38: // Left alt
            keyboard.left_alt = make;
            break;
        case 0x38 | 0x80: // Right alt
            keyboard.right_alt = make;
            break;
        case 0x3A: // Caps lock
            if (make) {
                keyboard.caps_lock = !keyboard.caps_lock;
            }
            break;
        case 0x45: // Num lock
            if (make) {
                keyboard.num_lock = !keyboard.num_lock;
            }
            break;
        case 0x46: // Scroll lock
            if (make) {
                keyboard.scroll_lock = !keyboard.scroll_lock;
            }
            break;
    }
    
    // Get ASCII character
    uint8_t ascii = 0;
    bool is_shift = keyboard.left_shift || keyboard.right_shift;
    bool is_caps = keyboard.caps_lock;
    
    if (scan_code < sizeof(scancode_to_ascii)) {
        ascii = is_shift ? scancode_to_ascii_shift[scan_code] : scancode_to_ascii[scan_code];
        
        // Handle caps lock
        if ((ascii >= 'a' && ascii <= 'z') || (ascii >= 'A' && ascii <= 'Z')) {
            if (is_caps) {
                ascii = (ascii >= 'a' && ascii <= 'z') ? ascii - 32 : ascii + 32;
            }
        }
    }
    
    // Add to buffer if there's space
    if (keyboard.buffer_count < 256) {
        keyboard.input_buffer[keyboard.buffer_tail] = ascii;
        keyboard.buffer_tail = (keyboard.buffer_tail + 1) % 256;
        keyboard.buffer_count++;
    }
    
    // Call callback if registered
    if (make) {
        if (keyboard.key_press_callback != NULL) {
            keyboard.key_press_callback(scan_code, ascii);
        }
    } else {
        if (keyboard.key_release_callback != NULL) {
            keyboard.key_release_callback(scan_code, ascii);
        }
    }
}

uint8_t keyboard_read(void) {
    if (!keyboard.initialized || keyboard.buffer_count == 0) {
        return 0;
    }
    
    uint8_t key = keyboard.input_buffer[keyboard.buffer_head];
    keyboard.buffer_head = (keyboard.buffer_head + 1) % 256;
    keyboard.buffer_count--;
    
    return key;
}

bool keyboard_available(void) {
    return keyboard.buffer_count > 0;
}

void keyboard_register_callbacks(void (*press_callback)(uint8_t, uint8_t), 
                                 void (*release_callback)(uint8_t, uint8_t)) {
    keyboard.key_press_callback = press_callback;
    keyboard.key_release_callback = release_callback;
}

bool keyboard_is_initialized(void) {
    return keyboard.initialized;
}

ternary_key_state_t keyboard_get_ternary_state(uint8_t scan_code) {
    if (!keyboard.initialized || scan_code >= 256) {
        return KEY_RELEASED;
    }
    return keyboard.key_states[scan_code];
}

void keyboard_handle_extended_scan_code(uint8_t scan_code) {
    // Handle extended scan codes (arrow keys, numpad, etc.)
    bool make = true;
    
    if (scan_code & 0x80) {
        make = false;
        scan_code &= 0x7F;
    }
    
    // Create extended scancode (E0 prefix + code)
    uint16_t extended_code = 0xE000 | scan_code;
    
    // Map extended keys to special values
    switch (extended_code) {
        case SCANCODE_ARROW_UP:
            // Arrow up = special character
            if (make && keyboard.buffer_count < 256) {
                keyboard.input_buffer[keyboard.buffer_tail] = 0x1E;  // Ctrl-P (custom)
                keyboard.buffer_tail = (keyboard.buffer_tail + 1) % 256;
                keyboard.buffer_count++;
            }
            break;
        case SCANCODE_ARROW_DOWN:
            if (make && keyboard.buffer_count < 256) {
                keyboard.input_buffer[keyboard.buffer_tail] = 0x1F;  // Ctrl-Q (custom)
                keyboard.buffer_tail = (keyboard.buffer_tail + 1) % 256;
                keyboard.buffer_count++;
            }
            break;
        case SCANCODE_ARROW_LEFT:
            if (make && keyboard.buffer_count < 256) {
                keyboard.input_buffer[keyboard.buffer_tail] = 0x1C;  // Ctrl-L (custom)
                keyboard.buffer_tail = (keyboard.buffer_tail + 1) % 256;
                keyboard.buffer_count++;
            }
            break;
        case SCANCODE_ARROW_RIGHT:
            if (make && keyboard.buffer_count < 256) {
                keyboard.input_buffer[keyboard.buffer_tail] = 0x1D;  // Ctrl-M (custom)
                keyboard.buffer_tail = (keyboard.buffer_tail + 1) % 256;
                keyboard.buffer_count++;
            }
            break;
    }
}

uint8_t keyboard_get_leds(void) {
    // Read LED state from keyboard controller
    // In real implementation, would query keyboard
    uint8_t leds = 0;
    if (keyboard.caps_lock) leds |= 0x04;
    if (keyboard.num_lock) leds |= 0x02;
    if (keyboard.scroll_lock) leds |= 0x01;
    return leds;
}

void keyboard_set_leds(uint8_t led_mask) {
    // Set LED state on keyboard
    // Would send command to keyboard controller
    keyboard.caps_lock = (led_mask & 0x04) != 0;
    keyboard.num_lock = (led_mask & 0x02) != 0;
    keyboard.scroll_lock = (led_mask & 0x01) != 0;
}

// =============================================================================
// PORT I/O
// =============================================================================

static inline void outb(uint16_t port, uint8_t value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void memset(void* ptr, int value, size_t size) {
    uint8_t* p = (uint8_t*)ptr;
    for (size_t i = 0; i < size; i++) {
        p[i] = value;
    }
}

