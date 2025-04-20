#ifndef LanternOS_Header_KERNEL_TYPES_H
#define LanternOS_Header_KERNEL_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define OS_NAME           "Lantern OS"
#define KERNEL_NAME       "Lantern Kernel"
#define KERNEL_VERSION    "v2025.4-1"
#define KERNEL_DATE       __DATE__

#define HALT() {asm volatile ("hlt");}
#define CLINT() {asm volatile ("cli");}

#endif
