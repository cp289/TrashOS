/**
 * vga.h: VGA text mode
 */

#ifndef _KERNEL_VGA_H
#define _KERNEL_VGA_H

/**
 * Hardware text mode color constants
 */
typedef enum
{
    VGA_COLOR_BLACK,
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
} vga_color_t;

void vga_init(void);
void vga_putc(char c);
void vga_puts(const char *data);
void vga_write(const char *data, size_t size);

#endif // _KERNEL_VGA_H
