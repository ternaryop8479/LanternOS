#ifndef LanternOS_Header_KERNEL_HAL_PORTS_H
#define LanternOS_Header_KERNEL_HAL_PORTS_H

#include <kernel/types.h>

// Ports API
void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);

// IO waiting
void io_wait();

#endif
