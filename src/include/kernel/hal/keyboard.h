#ifndef LanternOS_Header_KERNEL_HAL_KEYBOARD_H
#define LanternOS_Header_KERNEL_HAL_KEYBOARD_H

#include <kernel/types.h>
#include <stdbool.h>
#include <kernel/hal/ports.h>

void hal_keyboard_init(); // Initialize the keyboard
uint8_t hal_keyboard_get_state(); // Get current keyboard status
int hal_keyboard_read_scancode(); // Read the scan code

#endif
