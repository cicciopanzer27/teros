/**
 * @file console.c
 * @brief VGA text mode console driver implementation
 */

#include "console.h"

// Video memory
static uint16_t* video_memory = (uint16_t*)VGA_MEMORY;

// Cursor position
static size_t console_row = 0;
static size_t console_column = 0;

// Color
static uint8_t console_color = 0x07; // Light grey on black

// Combine color byte
static inline uint8_t make_color(uint8_t fg, uint8_t bg) {
    return fg | (bg << 4);
}

// Combine character and color into VGA entry
static inline uint16_t make_vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

void console_init(void) {
    console_clear();
}

void console_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            video_memory[index] = make_vga_entry(' ', console_color);
        }
    }
    console_row = 0;
    console_column = 0;
}

void console_set_color(uint8_t fg, uint8_t bg) {
    console_color = make_color(fg, bg);
}

void console_putchar(char c) {
    if (c == '\n') {
        console_column = 0;
        console_row++;
    } else if (c == '\t') {
        console_column = (console_column + 4) & ~(4 - 1);
    } else if (c == '\r') {
        console_column = 0;
    } else if (c >= ' ') {
        const size_t index = console_row * VGA_WIDTH + console_column;
        video_memory[index] = make_vga_entry(c, console_color);
        
        if (++console_column == VGA_WIDTH) {
            console_column = 0;
            console_row++;
        }
    }
    
    // Scroll if needed
    if (console_row == VGA_HEIGHT) {
        console_scroll();
    }
}

void console_puts(const char* str) {
    while (*str != '\0') {
        console_putchar(*str++);
    }
}

void console_write(const char* data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        console_putchar(data[i]);
    }
}

void console_scroll(void) {
    // Move all lines up by one
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index_to = (y - 1) * VGA_WIDTH + x;
            const size_t index_from = y * VGA_WIDTH + x;
            video_memory[index_to] = video_memory[index_from];
        }
    }
    
    // Clear last line
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        video_memory[index] = make_vga_entry(' ', console_color);
    }
    
    // Set cursor to last line
    console_row = VGA_HEIGHT - 1;
    console_column = 0;
}

