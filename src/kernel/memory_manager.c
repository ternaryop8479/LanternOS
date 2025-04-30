#include <kernel/stdio.h>
#include <kernel/memory_manager.h>

static unsigned char *mmap_ptr = NULL;
static uint64_t mmap_size = 0;

int mmap_init(EFI_SYSTEM_TABLE *systemTable) {
    EFI_STATUS status;
    EFI_MEMORY_DESCRIPTOR *memoryMap = NULL;
    UINTN mapKey = 0, descriptorSize = 0;
    uint32_t descriptorVersion = 0;

    status = systemTable->BootServices->GetMemoryMap(&mmap_size, memoryMap, &mapKey, &descriptorSize, &descriptorVersion);
    if(status != EFI_BUFFER_TOO_SMALL) {
        printlogf("Failed to initialize the memory map(first step): 0x%x", (int)status);
        while(1) HALT();
    }

    status = systemTable->BootServices->AllocatePool();
    return status;
}
