#ifndef LanternOS_Header_KERNEL_MEMORY_MANAGER_H
#define LanternOS_Header_KERNEL_MEMORY_MANAGER_H

#include <kernel/types.h>

int mmap_init(EFI_SYSTEM_TABLE *systemTable); // Initialize the memory table, WARN: This function will call systemTable->BootServices->ExitBootServices(), please call this function AT LAST OF INITIALIZATIONS!
bool is_mmap_ready(); // Check if the memory map is ready(initialized)
unsigned char *get_mmap(); // Get the pointer of memory map

#endif
