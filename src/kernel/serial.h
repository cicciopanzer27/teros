/**
 * @file serial.h
 * @brief Serial Port Driver Header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// =============================================================================
// TERNARY FLOW CONTROL STATES
// =============================================================================

// Ternary flow control state: -1 (stop), 0 (hold), +1 (go)
typedef int8_t flow_state_t;
#define FLOW_STOP  -1
#define FLOW_HOLD  0
#define FLOW_GO    +1

// =============================================================================
// SERIAL PORT CONSTANTS
// =============================================================================

#define SERIAL_COM1 0x3F8
#define SERIAL_COM2 0x2F8
#define SERIAL_COM3 0x3E8
#define SERIAL_COM4 0x2E8

// Parity options
#define SERIAL_PARITY_NONE 0
#define SERIAL_PARITY_ODD 1
#define SERIAL_PARITY_EVEN 2
#define SERIAL_PARITY_MARK 3
#define SERIAL_PARITY_SPACE 4

// =============================================================================
// SERIAL PORT STRUCTURE
// =============================================================================

typedef struct serial_port serial_state_t;

// =============================================================================
// SERIAL PORT FUNCTIONS
// =============================================================================

/**
 * @brief Initialize serial port
 * @param port Port address
 * @param baud_rate Baud rate
 * @param data_bits Data bits (5-8)
 * @param stop_bits Stop bits (1-2)
 * @param parity Parity mode
 */
void serial_init_port(uint16_t port, uint32_t baud_rate, uint8_t data_bits, 
                     uint8_t stop_bits, uint8_t parity);

/**
 * @brief Initialize serial driver (default COM1)
 */
void serial_init(void);

/**
 * @brief Put character to serial port
 * @param s Serial port
 * @param c Character
 */
void serial_putchar(serial_state_t* s, char c);

/**
 * @brief Get character from serial port
 * @param s Serial port
 * @return Character or 0
 */
char serial_getchar(serial_state_t* s);

/**
 * @brief Write data to serial port
 * @param s Serial port
 * @param data Data to write
 * @param size Size of data
 */
void serial_write(serial_state_t* s, const char* data, size_t size);

/**
 * @brief Write string to serial port
 * @param s Serial port
 * @param str String to write
 */
void serial_puts(serial_state_t* s, const char* str);

/**
 * @brief Read data from serial port
 * @param s Serial port
 * @param buffer Buffer to read into
 * @param size Maximum bytes to read
 * @return Number of bytes read
 */
size_t serial_read(serial_state_t* s, char* buffer, size_t size);

/**
 * @brief Get serial port structure
 * @param port Port address
 * @return Serial port structure or NULL
 */
serial_state_t* serial_get_port(uint16_t port);

/**
 * @brief Check if port is initialized
 * @param port Port address
 * @return true if initialized
 */
bool serial_is_initialized(uint16_t port);

/**
 * @brief Enable/disable interrupts on serial port
 * @param s Serial port
 * @param bypass enable Interrupts enabled?
 */
void serial_set_interrupts(serial_state_t* s, bool enable);

/**
 * @brief Get flow control state (ternary)
 * @param s Serial port
 * @return FLOW_STOP, FLOW_HOLD, or FLOW_GO
 */
flow_state_t serial_get_flow_state(serial_state_t* s);

/**
 * @brief Set flow control
 * @param s Serial port
 * @param rts RTS line state
 * @param dtr DTR line state
 */
void serial_set_flow_control(serial_state_t* s, bool rts, bool dtr);

#endif // SERIAL_H

