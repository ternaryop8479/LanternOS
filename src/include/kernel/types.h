#ifndef LanternOS_Header_KERNEL_TYPES_H
#define LanternOS_Header_KERNEL_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <efi.h>
#include <efiapi.h>
#include <efilib.h>

#define OS_NAME           "Lantern OS"
#define KERNEL_NAME       "Lantern Kernel"
#define KERNEL_VERSION    "v2025.4-2"
#define KERNEL_DATE       __DATE__

#define HALT() {asm volatile ("hlt");}
#define CLINT() {asm volatile ("cli");}

#endif
