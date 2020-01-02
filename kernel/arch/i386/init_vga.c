/**
 * init_vga.c: VGA text mode driver (init section debugging)
 *
 * For documentation, see: http://www.osdever.net/FreeVGA/vga/vga.htm
 */

#include "std.h"
#include "string.h"
#include "vga.h"

size_t INIT_VGA_HEIGHT = 25;
size_t INIT_VGA_WIDTH = 80;

static size_t vga_row = 0;
static size_t vga_col = 0;
static vga_color_t vga_color = VGA_COLOR_LIGHT_GREY | (vga_color_t)VGA_COLOR_BLACK << 4;

volatile uint8_t *init_vga_crtc_data = VGA_PADDR_CRTC_DATA;
vga_entry_t *init_vga_buffer = VGA_PADDR_BUFFER;

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
    const size_t index = y * INIT_VGA_WIDTH + x;
    init_vga_buffer[index] = vga_entry(c, color);
}

static void vga_scrolldown(size_t lines)
{
    for (size_t y = 0; y < INIT_VGA_HEIGHT - lines; y++) {
        for (size_t x = 0; x < INIT_VGA_WIDTH; x++) {
            const size_t to_index = y * INIT_VGA_WIDTH + x;
            const size_t from_index = to_index + lines * INIT_VGA_WIDTH;
            init_vga_buffer[to_index] = init_vga_buffer[from_index];
        }
    }
    for (size_t y = INIT_VGA_HEIGHT - lines; y < INIT_VGA_HEIGHT; y++) {
        for (size_t x = 0; x < INIT_VGA_WIDTH; x++) {
            init_vga_buffer[y * INIT_VGA_WIDTH + x] = ' ';
        }
    }
    vga_row -= lines;
}

static inline void vga_linebreak(void)
{
    vga_col = 0;
    if (++vga_row == INIT_VGA_HEIGHT)
        vga_scrolldown(1);
}

// GLOBAL FUNCTIONS

void init_vga_putc(char c)
{
    switch (c)
    {
        case '\n':
            vga_linebreak();
            break;
        default:
            vga_putentryat(c, vga_color, vga_col, vga_row);
            if (++vga_col == INIT_VGA_WIDTH)
                vga_linebreak();
    }
}

void init_vga_write(const char *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
        init_vga_putc(data[i]);
}

void init_vga_puts(const char *data)
{
    init_vga_write(data, strlen(data));
}

// Clear screen
void init_vga_clear(void)
{
    for (size_t y = 0; y < INIT_VGA_HEIGHT; y++) {
        for (size_t x = 0; x < INIT_VGA_WIDTH; x++) {
            const size_t index = y * INIT_VGA_WIDTH + x;
            init_vga_buffer[index] = vga_entry(' ', vga_color);
        }
    }
}

size_t init_vga_get_col(void)
{
    return vga_col;
}

size_t init_vga_get_row(void)
{
    return vga_row;
}
