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
#define KERNEL_VERSION    "v2025.5-1"
#define KERNEL_DATE       __DATE__

#define HALT() {asm volatile ("hlt");}
#define CLINT() {asm volatile ("cli");}

typedef unsigned long long KERNEL_STATUS;
#define ERROR(a) EFI_ERROR(a)
#define KERNEL_SUCCESS EFI_SUCCESS
#define KERNEL_ERROR_INVALID_MAGIC 0x7378867771670000
#define KERNEL_ERROR_INVALID_POINTER 0x7378868084820000
#define KERNEL_ERROR_FAILED_MALLOC 0x7065737677767967
#define KERNEL_ERROR_FAILED_SELFCHECK 0x7065737683706775

#endif
