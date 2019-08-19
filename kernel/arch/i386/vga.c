/**
 * vga.c: VGA text mode driver
 *
 * For documentation, see: http://www.osdever.net/FreeVGA/vga/vga.htm
 */

#include <stddef.h>
#include <stdint.h>

#include "string.h"
#include "vga.h"

size_t VGA_HEIGHT;
size_t VGA_WIDTH;

size_t vga_row;
size_t vga_col;
uint8_t vga_color;
uint16_t *vga_buffer;

static inline uint8_t vga_entry_color(vga_color_t fg, vga_color_t bg)
{
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
    return (uint16_t) uc | (uint16_t) color << 8;
}

static void vga_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    vga_buffer[index] = vga_entry(c, color);
}

static void vga_scrolldown(size_t lines)
{
    for (size_t y = 0; y < VGA_HEIGHT - lines; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t to_index = y * VGA_WIDTH + x;
            const size_t from_index = to_index + lines * VGA_WIDTH;
            vga_buffer[to_index] = vga_buffer[from_index];
        }
    }
}

static inline void vga_linebreak(void)
{
    vga_col = 0;
    if (++vga_row == VGA_HEIGHT)
        vga_scrolldown(1);
}

// GLOBAL FUNCTIONS

void vga_putc(char c)
{
    switch (c)
    {
        case '\n':
            vga_linebreak();
            break;
        default:
            vga_putentryat(c, vga_color, vga_col, vga_row);
            if (++vga_col == VGA_WIDTH)
                vga_linebreak();
    }
}

void vga_write(const char *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
        vga_putc(data[i]);
}

void vga_puts(const char *data)
{
    vga_write(data, strlen(data));
}

void vga_init(size_t r, size_t c)
{
    VGA_HEIGHT = r;
    VGA_WIDTH = c;
    vga_row = 0;
    vga_col = 0;
    vga_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_buffer = (void*) 0xB8000;

    // Clear screen
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            vga_buffer[index] = vga_entry(' ', vga_color);
        }
    }
}
