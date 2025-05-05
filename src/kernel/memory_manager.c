#include <kernel/hal/memory_map.h>
#include <kernel/memory_manager.h>
#include <kernel/stdio.h>
#include <kernel/types.h>

struct ALLOCATED_CHUNK {
    uint64_t beginAddress; // The begin address of chunk
    uint64_t pageNum; // Size of chunk(page)
    uint64_t appliedSize; // The size(byte) that user applied for
};
#define ALLOCATED_CHUNK struct ALLOCATED_CHUNK // You needn't write "struct" before "ALLOCATED_CHUNK" by that

static KERNEL_MMAP_PAGES *mmap_pages = NULL; // Memory map pages information (Just a struct pointer instead of array)
static bool *pages_available = NULL; // Mark if the page is available (Array)
static uint64_t TOTAL_AVAILABLE_PAGES_NUM = 0; // Total available(for system) memory size

// Calculate the page index that target address in
static uint64_t address_to_index(uint64_t addr) {
    uint64_t index = 0xFFFFFFFFFFFFFFFF;
    for(uint64_t i = 0; i < mmap_pages->page_num; ++i) {
        if(mmap_pages->page_bases[i] <= addr && mmap_pages->page_bases[i] + PAGE_SIZE > addr) {
            index = i;
            break;
        }
    }
    return index; // If index not found, it will return 0xFFFFFFFFFFFFFFFF
}

// You can allocate specified pages in a suitable sized free memory block(pages) by this function (but you need to add chunk information struct by yourself)
static void *allocate_pages(uint64_t pageNum) {
    // The address and the number of minimum continious pages
    uint64_t minPageIndex = 0xFFFFFFFFFFFFFFFF, minPageNum = 0xFFFFFFFFFFFFFFFF;

    // Erodic all pages
    for(uint64_t i = 0; i < mmap_pages->page_num; ++i) {
        if(!pages_available[i]) {
            continue;
        }

        const uint64_t currentChunkIndex = i; // Begin page index of current chunk
        uint64_t currentChunkPageNum = 0; // Current chunk size (page)
        for(const uint64_t pageName = mmap_pages->page_names[i]; mmap_pages->page_names[i] == pageName && pages_available[i]; ++currentChunkPageNum, ++i); // Calculate the size of current chunk
        if(currentChunkPageNum >= pageNum && currentChunkPageNum < minPageNum) { // Compare the number of pages of current chunk and the number of pages of minimum valid chunk
            minPageIndex = currentChunkIndex; // Record index
            minPageNum = currentChunkPageNum; // Record size
        }
    }

    // No suitable chunk found
    if(minPageIndex == 0xFFFFFFFFFFFFFFFF) {
        return NULL;
    }

    // Mark every used pages (needed pages(pageNum) instead of all pages in chunk(minPageSize))
    for(uint64_t i = minPageIndex; i - minPageIndex < pageNum; ++i) {
        pages_available[i] = false;
    }

    return (void *)mmap_pages->page_bases[minPageIndex];
}

static KERNEL_STATUS free_pages(uint64_t beginAddress, uint64_t pageNum) {
    // Find the page that beginAddress in
    uint64_t beginIndex = address_to_index(beginAddress);
    if(beginIndex == 0xFFFFFFFFFFFFFFFF) {
        return EFI_INVALID_PARAMETER;
    }

    // No page found (invalid beginAddress)
    if(beginIndex == 0xFFFFFFFFFFFFFFFF) {
        return EFI_INVALID_PARAMETER;
    }

    // Mark the pages free
    for(uint64_t i = beginIndex; i - beginIndex < pageNum; pages_available[i] = true, ++i);

    return KERNEL_SUCCESS;
}

KERNEL_STATUS mmanager_init(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable) {
    KERNEL_STATUS status;
    status = mmap_init(imageHandle, systemTable); // Initalize memory map
    if(ERROR(status)) {
        return status;
    }

    mmap_pages = get_mmap(); // Get memory map information struct
    const uint64_t pages_availableNeedPageNum = mmap_pages->page_num / PAGE_SIZE; // The number of array pages_available needed pages
    TOTAL_AVAILABLE_PAGES_NUM = mmap_pages->page_num; // Calculate total memory size

    bool found = false;
    uint64_t startPageIndex = 0xFFFFFFFFFFFFFFFF;
    for(uint64_t i = 0; i < mmap_pages->page_num; ++i) {
        startPageIndex = i;
        for(uint64_t pageName = mmap_pages->page_names[i], totalPages = 0; pageName == mmap_pages->page_names[i]; ((totalPages == pages_availableNeedPageNum) ? found = true : 0), ++i, ++totalPages);
        if(found) {
            pages_available = (bool *)mmap_pages->page_bases[startPageIndex];
            break;
        }
    }
    if(!found || startPageIndex == 0xFFFFFFFFFFFFFFFF) {
        printlogf("[mmanager_init()] Failed to allocate memory to pages info!");
        return KERNEL_ERROR_FAILED_MALLOC;
    }

    for(uint64_t i = 0; i < mmap_pages->page_num; pages_available[i] = true, ++i); // Clear the pages

    for(uint64_t i = startPageIndex; i - startPageIndex < pages_availableNeedPageNum; pages_available[i] = false, ++i); // Mark the used pages

    const char *testString = "Hello, LanternOS Memory Manager!"; // Test string
    const uint64_t size = strlen(testString); // String size
    unsigned char *stringMemory = (unsigned char *)kernel_malloc(size); // Alloc memory
    memcpy(stringMemory, testString, size);
    printlogf("[mmanager_init()] Test -- Size: %ulldbyte, Address: 0x%ullx, Content: %s", size, (uint64_t)stringMemory, stringMemory); // Log out information
    bool flag = true; // Check if every page flags is marked
    for(uint64_t i = address_to_index((uint64_t)stringMemory) - 1; i < size / PAGE_SIZE + 1; (pages_available[i] ? flag = false : 0), ++i);
    if(!flag) {
        printlogf("[mmanager_init()] Test failed: Malloc failed");
        return KERNEL_ERROR_FAILED_MALLOC;
    }
    printlogf("[mmanager_init()] Freeing memory...");
    kernel_free(stringMemory); // Free memory
    // Check if every page flags is unmarked
    for(uint64_t i = address_to_index((uint64_t)stringMemory) - 1; i < size / PAGE_SIZE + 1; (!pages_available[i] ? flag = false : 0), ++i);
    if(!flag) {
        printlogf("[mmanager_init()] Test failed: Free failed");
        return KERNEL_ERROR_FAILED_MALLOC;
    }
    printlogf("[mmanager_init()] Testing aligned_malloc...");
    stringMemory = kernel_aligned_alloc(size);
    if((uint64_t)stringMemory % PAGE_SIZE != 0) {
        printlogf("[mmanager_init()] Test failed: Memory is not aligned");
        return KERNEL_ERROR_FAILED_MALLOC;
    }
    kernel_free(stringMemory);
    for(uint64_t i = address_to_index((uint64_t)stringMemory) - 1; i < size / PAGE_SIZE + 1; (!pages_available[i] ? flag = false : 0), ++i);
    if(!flag) {
        printlogf("[mmanager_init()] Test failed: Free failed");
        return KERNEL_ERROR_FAILED_MALLOC;
    }

    return EFI_SUCCESS;
}

void *kernel_malloc(uint64_t size) {
    uint64_t needPagesNum; { // Calculate the number of needed pages
        if(size == 0) {
            needPagesNum = 1; // Allocate a available page for program
        } else {
            needPagesNum = (size + sizeof(ALLOCATED_CHUNK)) / PAGE_SIZE + 1; // Calculate the number of pages that needed
        }
    }

    void *ptr = allocate_pages(needPagesNum); // Allocate the pages that our need
    if(ptr == NULL) { // Failed to allocate the pages
        return NULL;
    }

    ((ALLOCATED_CHUNK *)ptr)->beginAddress = (uint64_t)ptr; // Record the begin address
    ((ALLOCATED_CHUNK *)ptr)->pageNum = needPagesNum; // Calculate the chunk size(page)
    ((ALLOCATED_CHUNK *)ptr)->appliedSize = size; // Record the size that user applied

    return ptr + sizeof(ALLOCATED_CHUNK);
}

void *kernel_aligned_alloc(uint64_t size) {
    uint64_t needPagesNum; { // Calculate the number of needed pages
        if(size == 0) {
            needPagesNum = 2; // Allocate a available page for program
        } else {
            needPagesNum = (size + sizeof(ALLOCATED_CHUNK)) / PAGE_SIZE + 2; // Calculate the number of pages that needed
        }
    }

    void *ptr = allocate_pages(needPagesNum); // Allocate the pages that our need
    if(ptr == NULL) { // Failed to allocate the pages
        return NULL;
    }

    ((ALLOCATED_CHUNK *)(ptr + (PAGE_SIZE - sizeof(ALLOCATED_CHUNK))))->beginAddress = (uint64_t)ptr; // Record the begin address
    ((ALLOCATED_CHUNK *)(ptr + (PAGE_SIZE - sizeof(ALLOCATED_CHUNK))))->pageNum = needPagesNum; // Calculate the chunk size(page)
    ((ALLOCATED_CHUNK *)(ptr + (PAGE_SIZE - sizeof(ALLOCATED_CHUNK))))->appliedSize = size; // Record the size that user applied

    return ptr + PAGE_SIZE;
}

KERNEL_STATUS kernel_free(void *ptr) {
    ALLOCATED_CHUNK *chunkInfo = (ALLOCATED_CHUNK *)(ptr - sizeof(ALLOCATED_CHUNK));
    return free_pages((uint64_t)ptr, chunkInfo->pageNum);
}

void *kernel_realloc(void *ptr, size_t size) {
    ALLOCATED_CHUNK *chunkInfo = (ALLOCATED_CHUNK *)(ptr - sizeof(ALLOCATED_CHUNK));
    uint64_t newPageNum = size / PAGE_SIZE + 1; // Calculate the number of new pages
    if(newPageNum == chunkInfo->pageNum) {
        return ptr;
    } else if(newPageNum < chunkInfo->pageNum) {
        uint64_t beginIndex = address_to_index((uint64_t)ptr);
        beginIndex += newPageNum; // The index of pages that not need
        free_pages(mmap_pages->page_bases[beginIndex], chunkInfo->pageNum - newPageNum);
        return ptr;
    } else { // newPageNum > chunkInfo->pageNum
        uint64_t beginIndex = address_to_index((uint64_t)ptr);
        beginIndex += chunkInfo->pageNum; // New begin

        // Check if is same block
        if(mmap_pages->page_names[beginIndex] != mmap_pages->page_names[beginIndex - 1]) {
            kernel_free(ptr);
            return kernel_malloc(size);
        }

        // Check if following memory pages is able to alloc
        bool canExpand = true;
        for(uint64_t i = beginIndex; mmap_pages->page_names[i] == mmap_pages->page_names[beginIndex - 1] && i - beginIndex < newPageNum - chunkInfo->pageNum; ++i) {
            if(!pages_available[i]) {
                canExpand = false;
                break;
            }
        }

        // Check if it can expand
        if(canExpand) {
            for(uint64_t i = beginIndex; i - beginIndex < newPageNum - chunkInfo->pageNum; pages_available[i] = false, ++i);
            return ptr;
        } else {
            kernel_free(ptr);
            return kernel_malloc(size);
        }
    }
}

uint64_t get_free_size() {
    uint64_t result = 0;
    for(uint64_t i = 0; i < mmap_pages->page_num; (pages_available[i] ? result += PAGE_SIZE : 0), ++i);
    return result;
}

uint64_t get_total_size() {
    return TOTAL_AVAILABLE_PAGES_NUM * PAGE_SIZE;
}
