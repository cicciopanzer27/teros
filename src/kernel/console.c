/**
 * @file console.c
 * @brief VGA Text Mode Console Driver
 * @author TEROS Development Team
 * @date 2025
 */

#include "console.h"
#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// VGA TEXT MODE IMPLEMENTATION
// =============================================================================

#define VGA_MEMORY 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_SIZE (VGA_WIDTH * VGA_HEIGHT)

// Colors
#define COLOR_BLACK 0
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_CYAN 3
#define COLOR_RED 4
#define COLOR_MAGENTA 5
#define COLOR_BROWN 6
#define COLOR_LIGHT_GREY 7
#define COLOR_DARK_GREY 8
#define COLOR_LIGHT_BLUE 9
#define COLOR_LIGHT_GREEN 10
#define COLOR_LIGHT_CYAN 11
#define COLOR_LIGHT_RED 12
#define COLOR_LIGHT_MAGENTA 13
#define COLOR_YELLOW 14
#define COLOR_WHITE 15

// VGA entry helper macros
#define MAKE_COLOR(fg, bg) ((uint8_t)(fg | (bg << 4)))
#define MAKE_ENTRY(c, color) ((uint16_t)(c | ((uint16_t)color << 8)))

typedef struct {
    uint8_t* buffer;
    size_t row;
    size_t column;
    uint8_t color;
    bool initialized;
} console_state_t;

static console_state_t console;

// =============================================================================
// CONSOLE FUNCTIONS
// =============================================================================

void console_init(void) {
    if (console.initialized) {
        return;
    }
    
    console.buffer = (uint8_t*)VGA_MEMORY;
    console.row = 0;
    console.column = 0;
    console.color = MAKE_COLOR(COLOR_WHITE, COLOR_BLACK);
    console.initialized = true;
    
    console_clear();
}

void console_clear(void) {
    if (!console.initialized) {
        return;
    }
    
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            console.buffer[index * 2] = ' ';
            console.buffer[index * 2 + 1] = console.color;
        }
    }
    
    console.column = 0;
    console.row = 0;
}

void console_setcolor(uint8_t color) {
    console.color = color;
}

void console_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    console.buffer[index * 2] = c;
    console.buffer[index * 2 + 1] = color;
}

void console_putchar(char c) {
    if (c == '\n') {
        console_newline();
    } else if (c == '\r') {
        console.column = 0;
    } else if (c == '\t') {
        console.column += 4;
        if (console.column >= VGA_WIDTH) {
            console_newline();
        }
    } else {
        console_putentryat(c, console.color, console.column, console.row);
        
        if (++console.column == VGA_WIDTH) {
            console_newline();
        }
    }
}

void console_newline(void) {
    console.column = 0;
    
    if (++console.row == VGA_HEIGHT) {
        console.row = 0;
    }
    
    // Clear the new line
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        console_putentryat(' ', console.color, x, console.row);
    }
}

void console_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        console_putchar(data[i]);
    }
}

void console_puts(const char* str) {
    console_write(str, strlen(str));
}

// Get current cursor position
size_t console_get_row(void) {
    return console.row;
}

size_t console_get_column(void) {
    return console.column;
}

// Set cursor position
void console_set_cursor(size_t row, size_t column) {
    console.row = row;
    console.column = column;
    
    if (console.row >= VGA_HEIGHT) {
        console.row = 0;
    }
    if (console.column >= VGA_WIDTH) {
        console.column = 0;
    }
}

// Move cursor
void console_move_cursor(int drow, int dcol) {
    int new_row = (int)console.row + drow;
    int new_col = (int)console.column + dcol;
    
    if (new_row < 0) new_row = 0;
    if (new_row >= VGA_HEIGHT) new_row = VGA_HEIGHT - 1;
    if (new_col < 0) new_col = 0;
    if (new_col >= VGA_WIDTH) new_col = VGA_WIDTH - 1;
    
    console.row = new_row;
    console.column = new_col;
}

// Scroll console
void console_scroll(int lines) {
    if (lines == 0) return;
    
    if (lines > 0) {
        // Scroll up
        for (size_t y = 0; y < VGA_HEIGHT - lines; y++) {
            for (size_t x = 0; x < VGA_WIDTH; x++) {
                size_t src_index = (y + lines) * VGA_WIDTH + x;
                size_t dst_index = y * VGA_WIDTH + x;
                console.buffer[dst_index * 2] = console.buffer[src_index * 2];
                console.buffer[dst_index * 2 + 1] = console.buffer[src_index * 2 + 1];
            }
        }
        
        // Clear bottom lines
        for (size_t y = VGA_HEIGHT - lines; y < VGA_HEIGHT; y++) {
            for (size_t x = 0; x < VGA_WIDTH; x++) {
                console_putentryat(' ', console.color, x, y);
            }
        }
        
        if (console.row >= VGA_HEIGHT - lines) {
            console.row = VGA_HEIGHT - lines - 1;
        }
    } else {
        // Scroll down
        lines = -lines;
        for (size_t y = VGA_HEIGHT - 1; y >= lines; y--) {
            for (size_t x = 0; x < VGA_WIDTH; x++) {
                size_t src_index = (y - lines) * VGA_WIDTH + x;
                size_t dst_index = y * VGA_WIDTH + x;
                console.buffer[dst_index * 2] = console.buffer[src_index * 2];
                console.buffer[dst_index * 2 + 1] = console.buffer[src_index * 2 + 1];
            }
        }
        
        // Clear top lines
        for (size_t y = 0; y < lines; y++) {
            for (size_t x = 0; x < VGA_WIDTH; x++) {
                console_putentryat(' ', console.color, x, y);
            }
        }
        
        console.row += lines;
        if (console.row >= VGA_HEIGHT) {
            console.row = VGA_HEIGHT - 1;
        }
    }
}

// Set background color
void console_set_background(uint8_t color) {
    uint8_t fg = console.color & 0x0F;
    console.color = MAKE_COLOR(fg, color);
}

// Set foreground color
void console_set_foreground(uint8_t color) {
    uint8_t bg = (console.color & 0xF0) >> 4;
    console.color = MAKE_COLOR(color, bg);
}

// Get color
uint8_t console_get_color(void) {
    return console.color;
}

// Check if initialized
bool console_is_initialized(void) {
    return console.initialized;
}

