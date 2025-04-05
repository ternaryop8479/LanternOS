#ifndef LanternOS_Header_KERNEL_TERMINAL_H
#define LanternOS_Header_KERNEL_TERMINAL_H

#include <kernel/types.h>
#include <kernel/vga/vga.h>
#include <kernel/stdio.h>

extern uint32_t terminal_cursor_color;
extern uint32_t terminal_background_color;
extern uint32_t terminal_foreground_color;

void draw_cursor(); // The function to print cursor
void getline(char *buffer); // The getline function with cursor render

#endif
