/**
 * vga.c: VGA text mode driver
 *
 * For documentation, see: http://www.osdever.net/FreeVGA/vga/vga.htm
 */

#include "std.h"
#include "string.h"
#include "vga.h"

static size_t VGA_WIDTH = VGA_WIDTH_DEFAULT;
static size_t VGA_SIZE = VGA_HEIGHT_DEFAULT * VGA_WIDTH_DEFAULT;

static size_t vga_pos = 0;
static vga_color_t vga_color = VGA_COLOR_DEFAULT;

// TODO
//static volatile uint8_t *vga_crtc_data = VGA_PADDR_CRTC_DATA;
static vga_entry_t *vga_buffer = VGA_PADDR_BUFFER;

static inline vga_color_t vga_entry_color(int fg, int bg)
{
    return fg | (vga_color_t)bg << 4;
}

static inline vga_entry_t vga_entry(unsigned char uc, vga_color_t color)
{
    return (vga_entry_t) uc | (vga_entry_t) color << 8;
}

static inline void vga_putentryat(char c, vga_color_t color, size_t pos)
{
    vga_buffer[pos] = vga_entry(c, color);
}

static void vga_scrolldown(size_t lines)
{
    size_t p;
    for (p = 0; p < VGA_SIZE - VGA_WIDTH * lines; p++)
        vga_buffer[p] = vga_buffer[p + lines * VGA_WIDTH];
    for ( ; p < VGA_SIZE; p++)
        vga_putentryat(' ', vga_color, p);
    vga_pos -= lines * VGA_WIDTH;
}

static inline void vga_linebreak(void)
{
    vga_pos += VGA_WIDTH - vga_pos % VGA_WIDTH;
    if (vga_pos == VGA_SIZE)
        vga_scrolldown(1);
}

// GLOBAL FUNCTIONS

void vga_putc(char c)
{
    // Handle the condition when printing was interrupted in another context
    // TODO this solution is a hasty workaround, better to implement a
    // client/server model for printk
    if (vga_pos >= VGA_SIZE)
        vga_scrolldown(1);

    switch (c)
    {
        case '\n':
            vga_linebreak();
            break;
        default:
            vga_putentryat(c, vga_color, vga_pos);
            vga_pos++;
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
    for (size_t p = 0; p < VGA_SIZE; p++)
        vga_putentryat(' ', vga_color, p);
}

// Reset VGA buffer
void vga_reset(void)
{
    vga_clear();
    vga_set_position(0, 0);
}

void vga_map_buffer(uintptr_t new_buffer)
{
    vga_buffer = (void*)new_buffer;
}

void * vga_get_buffer(void)
{
    return vga_buffer;
}

void vga_set_position(size_t row, size_t col)
{
    vga_pos = VGA_WIDTH * row + col;
}
