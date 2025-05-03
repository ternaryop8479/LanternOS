#include <kernel/memory_manager.h>

int mmanager_init(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable) {
    return mmap_init(imageHandle, systemTable);
}
