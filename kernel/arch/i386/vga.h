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

extern size_t VGA_HEIGHT;
extern size_t VGA_WIDTH;
extern volatile uint8_t *vga_crtc_data;
extern uint16_t *vga_buffer;

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

void vga_init(void);
void vga_putc(char c);
void vga_puts(const char *data);
void vga_write(const char *data, size_t size);

#endif // _KERNEL_VGA_H
