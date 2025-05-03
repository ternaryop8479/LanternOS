#include <kernel/io/serial.h>
#include <kernel/stdio.h>
#include <kernel/string.h>
#include <kernel/hal/memory_map.h>
#include <kernel/types.h>
#include <kernel/vga/vga.h>

#define KERNEL_VIRTUAL_ADDRESS 0xFFFF800000000000
static unsigned char *mmap_base_ptr = NULL;
static uint64_t mmap_size = 0;
static PAGE_TABLE_ENTRY *kernel_pml4 = NULL;

static bool is_valid_physical_address(uint64_t address) {
    if(address == 0) {
        return false;
    }
    if(address >= KERNEL_VIRTUAL_ADDRESS) {
        return false;
    }
    return true;
}

static KERNEL_STATUS phys_to_virt(uint64_t phys, uint64_t virt, uint64_t flags, EFI_SYSTEM_TABLE *systemTable) {
    EFI_STATUS status = EFI_SUCCESS;
    PAGE_TABLE_ENTRY *currentLevel;

    // Parse virtual address to index of all levels
    const uint64_t pml4Index = (virt >> 39) & 0x1FF;
    const uint64_t pdptIndex = (virt >> 30) & 0x1FF;
    const uint64_t pdIndex   = (virt >> 21) & 0x1FF;
    const uint64_t ptIndex   = (virt >> 12) & 0x1FF;

    // Make sure the physical address and the virutal address is aligned by page
    if((phys & 0xFFF) != 0 || (virt & 0xFFF) != 0) {
        return EFI_INVALID_PARAMETER;
    }

    // ==== Deal with PML4 level ====
    currentLevel = kernel_pml4;
    if(!currentLevel[pml4Index].present) {
        PAGE_TABLE_ENTRY *pdpt;
        status = systemTable->BootServices->AllocatePool(EfiLoaderData, PAGE_SIZE, (void **)&pdpt);
        if(ERROR(status)) {
            return status;
        }
        memset(pdpt, 0, PAGE_SIZE);

        // Set PML4 entry
        currentLevel[pml4Index].raw       = 0;
        currentLevel[pml4Index].phys_addr = (uint64_t)pdpt >> 12;
        currentLevel[pml4Index].present   = 1;
        currentLevel[pml4Index].writable  = 1;
    }
    currentLevel = (PAGE_TABLE_ENTRY *)(currentLevel[pml4Index].phys_addr << 12);
    if(!is_valid_physical_address((uint64_t)currentLevel)) {
        return KERNEL_ERROR_INVALID_POINTER;
    }

    // ==== Deal with PDPT level ====
    if(!currentLevel[pdptIndex].present) {
        PAGE_TABLE_ENTRY *pd;
        status = systemTable->BootServices->AllocatePool(EfiLoaderData, PAGE_SIZE, (void **)&pd);
        if(ERROR(status)) {
            return status;
        }
        memset(pd, 0, PAGE_SIZE);

        // Set PDPT entry
        currentLevel[pdptIndex].raw       = 0;
        currentLevel[pdptIndex].phys_addr = (uint64_t)pd >> 12;
        currentLevel[pdptIndex].present   = 1;
        currentLevel[pdptIndex].writable  = 1;
    }
    currentLevel = (PAGE_TABLE_ENTRY *)(currentLevel[pdptIndex].phys_addr << 12);
    if(!is_valid_physical_address((uint64_t)currentLevel)) {
        return KERNEL_ERROR_INVALID_POINTER;
    }

    // ==== Deal with PD level ====
    if (!currentLevel[pdIndex].present) {
        PAGE_TABLE_ENTRY *pt;
        status = systemTable->BootServices->AllocatePool(EfiLoaderData, PAGE_SIZE, (void**)&pt);
        if (EFI_ERROR(status)) {
            return status;
        }
        memset(pt, 0, PAGE_SIZE);

        // Set PDPT entry
        currentLevel[pdIndex].raw       = 0;
        currentLevel[pdIndex].phys_addr = (uint64_t)pt >> 12;
        currentLevel[pdIndex].present   = 1;
        currentLevel[pdIndex].writable  = 1;
    }
    currentLevel = (PAGE_TABLE_ENTRY *)(currentLevel[pdIndex].phys_addr << 12);
    if(!is_valid_physical_address((uint64_t)currentLevel)) {
        return KERNEL_ERROR_INVALID_POINTER;
    }

    // ==== Deal with PT level ====
    // Set final page table entry
    currentLevel[ptIndex].raw         = 0;
    currentLevel[ptIndex].phys_addr   = phys >> 12;
    currentLevel[ptIndex].present     = (flags & PAGE_PRESENT) ? 1 : 0;
    currentLevel[ptIndex].writable    = (flags & PAGE_WRITABLE) ? 1 : 0;
    currentLevel[ptIndex].user_access = (flags & PAGE_USER) ? 1 : 0;
    currentLevel[ptIndex].no_execute  = (flags & PAGE_NO_EXECUTE) ? 1 : 0;
    if(!is_valid_physical_address((uint64_t)currentLevel)) {
        return KERNEL_ERROR_INVALID_POINTER;
    }

    // Flush TLB entry(by invlpg command)
    asm volatile("invlpg (%0)" : : "r"(virt) : "memory");
    return EFI_SUCCESS;
}

KERNEL_STATUS mmap_init(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable) {
    EFI_STATUS status;
    EFI_MEMORY_DESCRIPTOR *mmap = NULL;
    uint64_t mapKey = 0, descriptorSize = 0, mmapBufferSize = 0;
    uint32_t descriptorVersion = 0;
    uint8_t retryCount = 0;
    uint64_t currentVirtualAddress = KERNEL_VIRTUAL_ADDRESS;

    // Initialize kernel PML4 table
    status = systemTable->BootServices->AllocatePool(EfiLoaderData, PAGE_SIZE, (void**)&kernel_pml4);
    if(EFI_ERROR(status)) {
        printlogf("Failed to allocate PML4 table: 0x%x", status);
        return status;
    }
    memset(kernel_pml4, 0, PAGE_SIZE);
    
    // Get memory map size
    status = systemTable->BootServices->GetMemoryMap(&mmapBufferSize, mmap, &mapKey, &descriptorSize, &descriptorVersion); // mmap is NULL
    if(status != EFI_BUFFER_TOO_SMALL) {
        systemTable->BootServices->FreePool(kernel_pml4);
        printlogf("Failed to initialize the memory map(first step): 0x%x", (int)status);
        return -1;
    }
    printlogf("[mmap_init()] Variables: mmapBufferSize: %ulld, mapKey: %ulld, descriptorSize: %ulld, descriptorVersion: %ud", mmapBufferSize, mapKey, descriptorSize, descriptorVersion);

    // Pre-allocate a area (size about 4 * descriptorSize)
    status = systemTable->BootServices->AllocatePool(EfiLoaderData, mmapBufferSize + 4 * descriptorSize, (void **)&mmap);
    if(EFI_ERROR(status)) {
        systemTable->BootServices->FreePool(kernel_pml4);
        printlogf("Failed to initialize the memory map(Not enough memory): 0x%x", (int)status);
        return status;
    }

    // Get completed memory map(try again up to three times)
    do {
        status = systemTable->BootServices->GetMemoryMap(&mmapBufferSize, mmap, &mapKey, &descriptorSize, &descriptorVersion); // mmap is NULL
        if(!EFI_ERROR(status)) {
            break;
        }
    } while(retryCount++ < 3);
    retryCount = 0;
    if(EFI_ERROR(status)) { // Still failed to GetMemoryMap()
        systemTable->BootServices->FreePool(mmap);
        systemTable->BootServices->FreePool(kernel_pml4);
        printlogf("Failed to initialize the memory map(failed to GetMemoryMap()): 0x%x", (int)status);
        return status;
    }

    // Map all conventional memory regions
    uint64_t numEntries = mmapBufferSize / descriptorSize;
    for(uint64_t i = 0; i < numEntries; ++i) {
        EFI_MEMORY_DESCRIPTOR *descriptor = (EFI_MEMORY_DESCRIPTOR *)((uint8_t *)mmap + i * descriptorSize);

        if(descriptor->Type != EfiConventionalMemory) { // Unavailable page
            continue;
        }

        uint64_t physStart = descriptor->PhysicalStart;
        uint64_t numPages = descriptor->NumberOfPages;
        printlogf("Physstart: 0x%ullx, numPages: %ulld", physStart, numPages);

        // Map each pages to continious virutual space
        for(uint64_t page = 0; page < numPages; ++page) {
            status = phys_to_virt(
                physStart + page * PAGE_SIZE,
                currentVirtualAddress + page * PAGE_SIZE,
                PAGE_PRESENT | PAGE_WRITABLE | PAGE_NO_EXECUTE,
                systemTable
            );
            if(ERROR(status)) {
                systemTable->BootServices->FreePool(mmap);
                systemTable->BootServices->FreePool(kernel_pml4);
                printlogf("Failed mapping physic pages to virtual pages(At 0x%llx): 0x%x", physStart + page * PAGE_SIZE, status);
                return status;
            }
        }

        // Update address
        currentVirtualAddress += numPages * PAGE_SIZE;
        mmap_size += numPages * PAGE_SIZE;
    }

    printlogf("Enable table");
    // Activate new page table
    asm volatile("mov %0, %%cr3" : : "r"(kernel_pml4));

    printlogf("finalize mmap");
    // Finalize memory map
    mmap_base_ptr = (uint8_t *)KERNEL_VIRTUAL_ADDRESS;

    printlogf("write");
    serial_print("write");
    // Check if the memory map is initalized
    const uint64_t checkSum = 0x1F2E3D4E5F847900; // Magic num
    uint64_t *testAddr = (uint64_t *)mmap_base_ptr;
    *testAddr = checkSum;
    serial_print("check");
    printlogf("Checking magic num: 0x%ullx", *testAddr);
    if(*testAddr != checkSum) {
        systemTable->BootServices->FreePool(mmap);
        systemTable->BootServices->FreePool(kernel_pml4);
        printlogf("Invalid magic num!");
        serial_print("invalid");
        return KERNEL_ERROR_INVALID_MAGIC;
    } else {
        serial_print("OK!");
    }
    *testAddr = 0;

    do {
        status = systemTable->BootServices->ExitBootServices(imageHandle, mapKey);
        if(!EFI_ERROR(status)) {
            break;
        }

        // Retry to get the memory map
        status = systemTable->BootServices->GetMemoryMap(&mmap_size, mmap, &mapKey, &descriptorSize, &descriptorVersion); // mmap is NULL
        if(EFI_ERROR(status)) {
            systemTable->BootServices->FreePool(mmap);
            systemTable->BootServices->FreePool(kernel_pml4);
            return status;
        }
    } while(retryCount++ < 3);

    if(EFI_ERROR(status)) {
        printlogf("Failed to initialize the memory map(latest step)");
        return status;
    }

    const char *testString = "Hello, Lantern OS!";
    const uint64_t testStringSize = strlen(testString);
    memcpy(mmap_base_ptr, testString, testStringSize);
    printlogf("[mmap_init()] Test string: %s", mmap_base_ptr);
    // Not fill this area by '\0' for test.

    return status;
}

bool is_mmap_ready() {
    return (mmap_base_ptr != NULL && mmap_size != 0);
}

uint8_t *get_mmap() {
    return mmap_base_ptr;
}

bool is_page_available(uint8_t *page_addr) {
    return false;
}
