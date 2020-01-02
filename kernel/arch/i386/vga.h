/**
 * vga.h: VGA text mode
 */

#ifndef _KERNEL_VGA_H
#define _KERNEL_VGA_H

#include "std.h"

// Important physical addresses
#define VGA_PADDR_CRTC_DATA  ((void*)0x3C0)
#define VGA_PADDR_BUFFER     ((void*)0xB8000)

typedef uint16_t vga_entry_t;
typedef uint8_t vga_color_t;

#define VGA_HEIGHT_DEFAULT  25
#define VGA_WIDTH_DEFAULT   80
extern size_t VGA_HEIGHT;
extern size_t VGA_WIDTH;

// Hardware text mode color constants
enum {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE,
    VGA_COLOR_GREEN,
    VGA_COLOR_CYAN,
    VGA_COLOR_RED,
    VGA_COLOR_MAGENTA,
    VGA_COLOR_BROWN,
    VGA_COLOR_LIGHT_GREY,
    VGA_COLOR_DARK_GREY,
    VGA_COLOR_LIGHT_BLUE,
    VGA_COLOR_LIGHT_GREEN,
    VGA_COLOR_LIGHT_CYAN,
    VGA_COLOR_LIGHT_RED,
    VGA_COLOR_LIGHT_MAGENTA,
    VGA_COLOR_LIGHT_BROWN,
    VGA_COLOR_WHITE,
};

#define VGA_COLOR_DEFAULT   ((vga_color_t)(VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4))

void vga_clear(void);
void vga_map_buffer(uintptr_t new_buffer);
void vga_putc(char c);
void vga_puts(const char *data);
void vga_set_position(size_t row, size_t col);
void vga_write(const char *data, size_t size);

// VGA methods for init section
void init_vga_clear(void);
size_t init_vga_get_col(void);
size_t init_vga_get_row(void);
void init_vga_putc(char c);
void init_vga_puts(const char *data);
void init_vga_write(const char *data, size_t size);

#endif // _KERNEL_VGA_H
