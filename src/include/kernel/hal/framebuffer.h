#ifndef LanternOS_Header_KERHEL_HAL_FRAMEBUFFER_H
#define LanternOS_Header_KERHEL_HAL_FRAMEBUFFER_H

#include <kernel/types.h>
#include <boot/multiboot2.h>

// Framebuffer info
extern uint32_t FRAMEBUFFER_HEIGHT;
extern uint32_t FRAMEBUFFER_WIDTH;

int is_framebuffer_inited(); // Is framebuffer initialized
int framebuffer_init(multiboot_uint8_t *multiboot_info); // Initialize framebuffer
void framebuffer_set_pixel(uint32_t x, uint32_t y, uint32_t color); // Set pixel of the screen
uint32_t *get_framebuffer_ptr(); // A DANGERUS function to get the base address of framebuffer

#endif
