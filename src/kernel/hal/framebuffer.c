#include <kernel/hal/framebuffer.h>
#include <kernel/stdio.h>

static uint32_t *framebuffer_base = NULL;
uint32_t FRAMEBUFFER_HEIGHT = 0;
uint32_t FRAMEBUFFER_WIDTH = 0;

// Dangerus! may overflow
uint32_t *get_framebuffer_ptr() {
    return framebuffer_base;
}

// Check if the framebuffer is initialized
int is_framebuffer_inited() {
    return FRAMEBUFFER_WIDTH != 0;
}

// Set the pixel of the framebuffer
void framebuffer_set_pixel(uint32_t x, uint32_t y, uint32_t color) {
    framebuffer_base[y * FRAMEBUFFER_WIDTH + x] = color;
}

static void get_edid_resolution(EFI_SYSTEM_TABLE *systemTable, uint32_t *width, uint32_t *height) {
    *width = 0, *height = 0; // Initalize variables

    // Get EDID discovered
    EFI_STATUS status;
    EFI_EDID_DISCOVERED_PROTOCOL *EDP;
    uint32_t EDIDSize;
    uint8_t *EDIDData;

    EFI_EDID_ACTIVE_PROTOCOL *EAP;
    EFI_GUID EAP_GUID = EFI_EDID_ACTIVE_PROTOCOL_GUID;
    status = systemTable->BootServices->LocateProtocol(&EAP_GUID, NULL, (void**)&EAP);
    if(EFI_ERROR(status)) {
        printlogf("[Error] Failed to process EDID(EFI_EDID_ACTIVE_PROTOCOL).");
    }

    EFI_GUID EDP_GUID = EFI_EDID_DISCOVERED_PROTOCOL_GUID;
    status = systemTable->BootServices->LocateProtocol(&EDP_GUID, NULL, (void **)&EDP);
    if(EFI_ERROR(status)) {
        printlogf("[Error] Failed to process EDID: No EFI_EDID_DISCOVERED_PROTOCOL found.\n");
        return;
    }

    EDIDSize = EDP->SizeOfEdid;
    EDIDData = EDP->Edid;

    if(EDIDSize < 128 || EDIDData == NULL) {
        printlogf("[Error] Failed to process EDID.\n");
        return;
    }

    for(UINTN i = 72; i < EDIDSize; i += 18) {
        uint8_t descriptorType = EDIDData[i];
        if(descriptorType == 0x01) {
            uint16_t horizontal = (EDIDData[i + 2] << 4) | (EDIDData[i + 3] >> 4);
            uint16_t vertical = ((EDIDData[i + 3] & 0x0F) << 8) | EDIDData[i + 4];
            printf("%udx%ud\n", horizontal, vertical);
        }
    }
}

// Initialize the framebuffer
int framebuffer_init(EFI_SYSTEM_TABLE *systemTable) {
    // Get graphics output protocol
    EFI_GRAPHICS_OUTPUT_PROTOCOL *GOP;
    EFI_GUID GOPGUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_STATUS status = systemTable->BootServices->LocateProtocol(&GOPGUID, NULL, (void **)&GOP);
    if(EFI_ERROR(status) || !GOP) {
        return status;
    }

    // Initialize framebuffer base
    framebuffer_base = (uint32_t *)GOP->Mode->FrameBufferBase;
    uint32_t width, height;
    get_edid_resolution(systemTable, &width, &height);

    // Width & Height
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info = GOP->Mode->Info;
    FRAMEBUFFER_WIDTH = info->HorizontalResolution;
    FRAMEBUFFER_HEIGHT = info->VerticalResolution;


    return 0;
}
