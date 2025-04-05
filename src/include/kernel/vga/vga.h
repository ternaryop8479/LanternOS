#ifndef LanternOS_Header_KERNEL_VGA_VGA_H
#define LanternOS_Header_KERNEL_VGA_VGA_H

#include <kernel/types.h>
#include <kernel/hal/framebuffer.h>
#include <kernel/string.h>

#define VGA_WIDTH FRAMEBUFFER_WIDTH
#define VGA_HEIGHT FRAMEBUFFER_HEIGHT

#define VGA_UPCHAR 0
#define VGA_DOWNCHAR 1
#define VGA_LEFTCHAR 2
#define VGA_RIGHTCHAR 3

// External variables
extern uint32_t vga_background_color; // Background color for VGA texts
extern uint32_t vga_foreground_color;

// VGA graphic api
int vga_set_pixel(uint32_t x, uint32_t y, uint32_t color); // Use framebuffer_set_pixel() from kernel/hal/framebuffer.h
int vga_clear_screen(uint32_t color); // Clear screen

// VGA text api
int vga_move_cursor(uint8_t order); // Move cursor
void vga_backspace(); // Put backspace
int vga_putc(char c);
int vga_putstr(const char *str, uint64_t strSize);

#endif
