#ifndef LanternOS_Header_KERNEL_STDIO_H
#define LanternOS_Header_KERNEL_STDIO_H

#include <kernel/io/serial.h>
#include <kernel/io/keyboard.h>
#include <kernel/time.h>
#include <kernel/vga/vga.h>
#include <kernel/hal/ports.h>

typedef uint64_t toSize_t;

toSize_t printlogf(const char *format, ...); // Log function (wrap)
toSize_t printlogf_nn(const char *format, ...); // Log function (no-wrap)
toSize_t printf(const char *format, ...); // Print format string

char getch(); // Get a character from keyboard buffer

#endif
