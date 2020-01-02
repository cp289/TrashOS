/**
 * vga.c: VGA text mode driver
 *
 * For documentation, see: http://www.osdever.net/FreeVGA/vga/vga.htm
 */

#include "std.h"
#include "string.h"
#include "vga.h"

size_t VGA_HEIGHT = VGA_HEIGHT_DEFAULT;
size_t VGA_WIDTH = VGA_WIDTH_DEFAULT;

static size_t vga_row = 0;
static size_t vga_col = 0;
static vga_color_t vga_color = VGA_COLOR_DEFAULT;

volatile uint8_t *vga_crtc_data = VGA_PADDR_CRTC_DATA;
vga_entry_t *vga_buffer = VGA_PADDR_BUFFER;

static inline vga_color_t vga_entry_color(int fg, int bg)
{
    return fg | (vga_color_t)bg << 4;
}

static inline vga_entry_t vga_entry(unsigned char uc, vga_color_t color)
{
    return (vga_entry_t) uc | (vga_entry_t) color << 8;
}

static void vga_putentryat(char c, vga_color_t color, size_t x, size_t y)
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
    for (size_t y = VGA_HEIGHT - lines; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[y * VGA_WIDTH + x] = ' ';
        }
    }
    vga_row -= lines;
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

// Clear screen
void vga_clear(void)
{
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            vga_buffer[index] = vga_entry(' ', vga_color);
        }
    }
}

void vga_map_buffer(uintptr_t new_buffer)
{
    vga_buffer = (void*)new_buffer;
}

void vga_set_position(size_t row, size_t col)
{
    vga_row = row;
    vga_col = col;
}
