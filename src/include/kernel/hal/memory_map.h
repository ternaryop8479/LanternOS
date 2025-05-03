#ifndef LanternOS_Header_KERNEL_HAL_MEMORY_MAP_H
#define LanternOS_Header_KERNEL_HAL_MEMORY_MAP_H

#include <kernel/types.h>

static const uint64_t PAGE_SIZE = EFI_PAGE_SIZE; // Page size

// Page structure
#define PAGE_PRESENT     (1 << 0)
#define PAGE_WRITABLE    (1 << 1)
#define PAGE_USER        (1 << 2)
#define PAGE_NO_EXECUTE  (1ull << 63)

// Page table structure
typedef union {
    struct {
        uint64_t present         : 1;
        uint64_t writable        : 1;
        uint64_t user_access     : 1;
        uint64_t write_through   : 1;
        uint64_t cache_disable   : 1;
        uint64_t accessed        : 1;
        uint64_t dirty           : 1;
        uint64_t page_size       : 1; // 0 stands for 4KB page
        uint64_t global          : 1;
        uint64_t ignored         : 3;
        uint64_t phys_addr       : 40;
        uint64_t reserved        : 11;
        uint64_t no_execute      : 1;
    };
    uint64_t raw;
} PAGE_TABLE_ENTRY;

KERNEL_STATUS mmap_init(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable); // Initialize the memory table, WARN: This function will call systemTable->BootServices->ExitBootServices(), please call this function AT LAST OF INITIALIZATIONS!
bool is_mmap_ready(); // Check if the memory map is ready(initialized)
uint8_t *get_mmap(); // Get the pointer of memory map
bool is_page_available(); // Check if the page is available (for hardware)

#endif
