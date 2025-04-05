#ifndef LanternOS_Header_KERNEL_IO_KEYBOARD_H
#define LanternOS_Header_KERNEL_IO_KEYBOARD_H

#include <kernel/types.h>
#include <kernel/hal/keyboard.h>

char scancode_to_char(uint8_t scancode); // Translate scancode to character
void keyboard_init(); // Initialize the keyboard
void checkch(char *map); // Check the downed keys

#endif
