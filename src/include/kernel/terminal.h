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
void escape_to_normal(char *normal_str, const char *escape_str, uint64_t max_buffer_size); // The function to parse escape string
void normal_to_escape(char *escape_str, const char *normal_str, uint64_t max_buffer_size); // The function to convert normal string to escape string.
void parse_commandline(const char *commandline, char *params, uint64_t max_buffer_size); // The function to parse command line to params, and this function split params by charater '\n', and use "escape string"(such as "\\n" or "\\0") to describe param.
bool parse_param(char *param, const char *params, uint64_t index); // The function to parse target param

#endif
