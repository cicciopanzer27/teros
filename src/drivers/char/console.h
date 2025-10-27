/**
 * @file console.h
 * @brief Console driver header for TEROS
 */

#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>
#include <stddef.h>

// VGA Text Mode constants
#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

// Colors
#define COLOR_BLACK        0
#define COLOR_BLUE         1
#define COLOR_GREEN        2
#define COLOR_CYAN         3
#define COLOR_RED          4
#define COLOR_MAGENTA      5
#define COLOR_BROWN        6
#define COLOR_LIGHT_GREY   7
#define COLOR_DARK_GREY    8
#define COLOR_LIGHT_BLUE   9
#define COLOR_LIGHT_GREEN  10
#define COLOR_LIGHT_CYAN   11
#define COLOR_LIGHT_RED    12
#define COLOR_LIGHT_MAGENTA 13
#define COLOR_YELLOW       14
#define COLOR_WHITE        15

// Function prototypes
void console_init(void);
void console_clear(void);
void console_set_color(uint8_t fg, uint8_t bg);
void console_putchar(char c);
void console_puts(const char* str);
void console_write(const char* data, size_t length);
void console_scroll(void);
void console_move_cursor(size_t row, size_t col);
void console_get_cursor(size_t* row, size_t* col);
void console_set_attribute(uint8_t attr);

// Advanced functions
void console_printf(const char* format, ...);
void console_vprintf(const char* format, va_list args);
void console_put_number(int value, int base);
void console_put_hex(uint32_t value);
void console_put_dec(int value);

#endif // CONSOLE_H

