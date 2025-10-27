/**
 * @file console.h
 * @brief VGA Text Mode Console Header
 * @author TEROS Development Team
 * @date 2025
 */

#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// =============================================================================
// CONSOLE FUNCTIONS
// =============================================================================

/**
 * @brief Initialize console
 */
void console_init(void);

/**
 * @brief Clear console
 */
void console_clear(void);

/**
 * @brief Set console color
 * @param color Color to set
 */
void console_setcolor(uint8_t color);

/**
 * @brief Put character at specific position
 * @param c Character
 * @param color Color
 * @param x X position
 * @param y Y position
 */
void console_putentryat(char c, uint8_t color, size_t x, size_t y);

/**
 * @brief Put character
 * @param c Character
 */
void console_putchar(char c);

/**
 * @brief New line
 */
void console_newline(void);

/**
 * @brief Write data
 * @param data Data to write
 * @param size Size of data
 */
void console_write(const char* data, size_t size);

/**
 * @brief Write string
 * @param str String to write
 */
void console_puts(const char* str);

/**
 * @brief Get current row
 * @return Current row
 */
size_t console_get_row(void);

/**
 * @brief Get current column
 * @return Current column
 */
size_t console_get_column(void);

/**
 * @brief Set cursor position
 * @param row Row
 * @param column Column
 */
void console_set_cursor(size_t row, size_t column);

/**
 * @brief Move cursor
 * @param drow Row delta
 * @param dcol Column delta
 */
void console_move_cursor(int drow, int dcol);

/**
 * @brief Scroll console
 * @param lines Number of lines to scroll (positive = up, negative = down)
 */
void console_scroll(int lines);

/**
 * @brief Set background color
 * @param color Background color
 */
void console_set_background(uint8_t color);

/**
 * @brief Set foreground color
 * @param color Foreground color
 */
void console_set_foreground(uint8_t color);

/**
 * @brief Get current color
 * @return Current color
 */
uint8_t console_get_color(void);

/**
 * @brief Check if console is initialized
 * @return true if initialized
 */
bool console_is_initialized(void);

#endif // CONSOLE_H

