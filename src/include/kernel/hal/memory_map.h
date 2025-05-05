#ifndef LanternOS_Header_KERNEL_HAL_MEMORY_MAP_H
#define LanternOS_Header_KERNEL_HAL_MEMORY_MAP_H

#include <kernel/types.h>

static const uint64_t PAGE_SIZE = EFI_PAGE_SIZE;
struct KERNEL_MMAP_PAGES {
    uint64_t page_num;    // Number of pages
    uint64_t *page_bases; // Base addresses of pages
    uint64_t *page_names; // Name number of pages (Adjacent pages will use a same name number)
};
#define KERNEL_MMAP_PAGES struct KERNEL_MMAP_PAGES // You needn't to write "struct" before "KERNEL_MMAP_PAGES" by that.

KERNEL_STATUS mmap_init(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable); // Initialize the memory table, WARN: This function will call systemTable->BootServices->ExitBootServices(), please call this function AT LAST OF INITIALIZATIONS!
KERNEL_MMAP_PAGES *get_mmap(); // Get the pointer of memory map (NULL returned if mmap is not initialized)

#endif
