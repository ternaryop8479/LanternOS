#ifndef LanternOS_Header_KERNEL_STDIO_H
#define LanternOS_Header_KERNEL_STDIO_H

#include <kernel/io/serial.h>
#include <kernel/io/keyboard.h>
#include <kernel/time.h>
#include <kernel/vga/vga.h>
#include <kernel/hal/ports.h>

typedef uint64_t toSize_t;

toSize_t printf(const char *format, ...); // Print format string
toSize_t printlogf(const char *format, ...); // Log function (wrap)
toSize_t neprintlogf(const char *format, ...); // Log function (no-wrap)

// Variable arguments version
toSize_t vprintf(const char *format, va_list args);
toSize_t vprintlogf(const char *format, va_list args);
toSize_t vneprintlogf(const char *format, va_list args);

char getch(); // Get a character from keyboard buffer

#endif
