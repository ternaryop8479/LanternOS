#include <kernel/io/serial.h>
#include <kernel/stdio.h>
#include <kernel/string.h>
#include <kernel/hal/memory_map.h>
#include <kernel/types.h>
#include <kernel/vga/vga.h>

static KERNEL_MMAP_PAGES mmap_pages;

static EFI_STATUS GetMemoryMap_Retrying(EFI_SYSTEM_TABLE *systemTable, uint64_t *mmapBufferSize, EFI_MEMORY_DESCRIPTOR *mmap, uint64_t *mapKey, uint64_t *descriptorSize, uint32_t *descriptorVersion) {
    uint8_t retryCount = 0;
    EFI_STATUS status;
    while(retryCount++ < 2) {
        status = systemTable->BootServices->GetMemoryMap(mmapBufferSize, mmap, mapKey, descriptorSize, descriptorVersion);
        if(!EFI_ERROR(status)) {
            break;
        }
    }
    return status;
}

KERNEL_STATUS mmap_init(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable) {
    EFI_STATUS status;
    EFI_MEMORY_DESCRIPTOR *mmap = NULL;
    uint64_t mapKey = 0, descriptorSize = 0, mmapBufferSize = 0;
    uint32_t descriptorVersion = 0;

    // Get memory map size
    status = systemTable->BootServices->GetMemoryMap(&mmapBufferSize, mmap, &mapKey, &descriptorSize, &descriptorVersion); // mmap is NULL
    if(status != EFI_BUFFER_TOO_SMALL) {
        printlogf("[mmap_init()] Failed to initialize the memory map(first step): 0x%x", (int)status);
        return -1;
    }

    // Pre-allocate a area (size about 4 * descriptorSize)
    status = systemTable->BootServices->AllocatePool(EfiLoaderData, mmapBufferSize + 4 * descriptorSize, (void **)&mmap);
    if(EFI_ERROR(status)) {
        printlogf("[mmap_init()] Failed to initialize the memory map(Not enough memory): 0x%ullx", status);
        return status;
    }

    // Get completed memory map(try again up to two times)
    status = GetMemoryMap_Retrying(systemTable, &mmapBufferSize, mmap, &mapKey, &descriptorSize, &descriptorVersion);
    if(EFI_ERROR(status)) { // Still failed to GetMemoryMap()
        systemTable->BootServices->FreePool(mmap);
        printlogf("[mmap_init()] Failed to initialize the memory map(failed to GetMemoryMap()): 0x%ullx", status);
        return status;
    }

    // Calculate and allocate the memory for the page pointers
    uint64_t page_ptrs_size = 0;
    for(uint64_t i = 0; i < mmapBufferSize / descriptorSize; page_ptrs_size += (((EFI_MEMORY_DESCRIPTOR *)((uint8_t *)mmap + i * descriptorSize))->NumberOfPages * sizeof(uint64_t)), ++i);
    printlogf("[mmap_init()] Memory map table size: %ulld, total size: %ulld", page_ptrs_size, page_ptrs_size * 2);
    status = systemTable->BootServices->AllocatePool(EfiLoaderData, page_ptrs_size, (void **)&mmap_pages.page_bases);
    if(EFI_ERROR(status)) {
        systemTable->BootServices->FreePool(mmap);
        printlogf("[mmap_init()] Failed to allocate memory for page struct (mmap_pages.page_bases): 0x%ullx", status);
        return status;
    }
    status = systemTable->BootServices->AllocatePool(EfiLoaderData, page_ptrs_size, (void **)&mmap_pages.page_names);
    if(EFI_ERROR(status)) {
        systemTable->BootServices->FreePool(mmap);
        printlogf("[mmap_init()] Failed to allocate memory for page struct (mmap_pages.page_names): 0x%ullx", status);
        return status;
    }

    // Get completed memory map(try again up to two times)
    status = GetMemoryMap_Retrying(systemTable, &mmapBufferSize, mmap, &mapKey, &descriptorSize, &descriptorVersion);
    if(EFI_ERROR(status)) { // Still failed to GetMemoryMap()
        systemTable->BootServices->FreePool(mmap);
        systemTable->BootServices->FreePool(mmap_pages.page_bases);
        printlogf("[mmap_init()] Failed to initialize the memory map(failed to GetMemoryMap()): 0x%ullx", status);
        return status;
    }

    // Map all conventional memory regions
    for(uint64_t i = 0, pageIndex = 0, pageName = 1; i < mmapBufferSize / descriptorSize; ++i) {
        EFI_MEMORY_DESCRIPTOR *descriptor = (EFI_MEMORY_DESCRIPTOR *)((uint8_t *)mmap + i * descriptorSize);
        if(descriptor->Type != EfiConventionalMemory) { // Unavailable page
            continue;
        }
        printlogf("[mmap_init()] Block info -- Physical Address: 0x%ullx, Number of pages: %ulld", descriptor->PhysicalStart, descriptor->NumberOfPages);

        mmap_pages.page_num += descriptor->NumberOfPages; // Add the number of available pages to mmap_pages.page_num
        for(uint64_t j = 0; j < descriptor->NumberOfPages; ++j, ++pageIndex) { // Calculate the page bases for every pages
            mmap_pages.page_names[pageIndex] = pageName;
            mmap_pages.page_bases[pageIndex] = descriptor->PhysicalStart + j * PAGE_SIZE;
        }
        ++pageName;
    }

    status = systemTable->BootServices->ExitBootServices(imageHandle, mapKey);
    if(EFI_ERROR(status)) {
        printlogf("[mmap_init()] Failed to initialize the memory map(latest step)");
        return status;
    }

    // Write magic num and check
    const uint64_t MAGIC_NUM = 0x7684827877716787; // ASCII: "LTRNMGC" 87 (stands for 'T' and 'O',ascii 84 79)
    uint64_t *magicBase = (uint64_t *)mmap_pages.page_bases[0];
    *magicBase = MAGIC_NUM;
    if(*magicBase != MAGIC_NUM) {
        printlogf("[mmap_init()] Failed to verify the magic num: %ullx", *magicBase);
        return status;
    }

    // Write test string
    const char *testString = "Hello, Lantern OS!";
    const uint64_t testStringSize = strlen(testString);
    char *targetAddr = (char *)mmap_pages.page_bases[0];
    memcpy(targetAddr, testString, testStringSize);
    printlogf("[mmap_init()] Test string: %s", targetAddr);
    // Not fill this area by '\0' for test.

    return status;
}

KERNEL_MMAP_PAGES *get_mmap() {
    return &mmap_pages;
}
