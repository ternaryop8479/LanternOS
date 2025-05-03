#ifndef LanternOS_Header_KERNEL_MEMORY_MANAGER_H
#define LanternOS_Header_KERNEL_MEMORY_MANAGER_H

#include <kernel/types.h>
#include <kernel/hal/memory_map.h>

int mmanager_init(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable);

#endif
