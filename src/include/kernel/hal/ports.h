#ifndef LanternOS_Header_KERNEL_HAL_PORTS_H
#define LanternOS_Header_KERNEL_HAL_PORTS_H

#include <kernel/types.h>

// Ports API
void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);

void outw(uint16_t port, uint16_t value);
uint16_t inw(uint16_t port);

void outl(uint16_t port, uint32_t value);
uint32_t inl(uint16_t port);

// IO waiting
void io_wait();

#endif
