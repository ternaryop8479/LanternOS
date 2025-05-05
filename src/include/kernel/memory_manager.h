#ifndef LanternOS_Header_KERNEL_MEMORY_MANAGER_H
#define LanternOS_Header_KERNEL_MEMORY_MANAGER_H

#include <kernel/hal/memory_map.h>

KERNEL_STATUS mmanager_init(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable);
void *kernel_malloc(uint64_t size);
void *kernel_realloc(void *ptr, uint64_t size);
void *kernel_aligned_alloc(uint64_t size);
KERNEL_STATUS kernel_free(void *ptr);
uint64_t get_free_size();
uint64_t get_total_size();

#endif
