#ifndef LanternOS_Header_KERNEL_TYPES_H
#define LanternOS_Header_KERNEL_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <efi.h>
#include <efiapi.h>
#include <efilib.h>

#define OS_NAME           "Lantern OS"
#define KERNEL_NAME       "Lantern Kernel"
#define KERNEL_VERSION    "v2025.4-2"
#define KERNEL_DATE       __DATE__

#define HALT() {asm volatile ("hlt");}
#define CLINT() {asm volatile ("cli");}

typedef unsigned long long KERNEL_STATUS;
#define ERROR(a) EFI_ERROR(a)
#define KERNEL_ERROR_INVALID_MAGIC 0x737886777167
#define KERNEL_ERROR_INVALID_POINTER 0x737886808482

#endif
