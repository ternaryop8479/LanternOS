#ifndef LanternOS_Header_KERNEL_IO_SERIAL_H
#define LanternOS_Header_KERNEL_IO_SERIAL_H

#include <kernel/types.h>
#include <kernel/hal/ports.h>
#include <stdint.h>

// Initialize serial port, return 0 means successfully done.
int serial_init(void);

// Serial put functions
void serial_putc(char c); // Put character
void serial_print(const char* str); // Put string
void serial_write(const char *data, uint64_t dataSize); // Put string with target size

#endif
