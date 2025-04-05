#include <kernel/hal/framebuffer.h>

static uint32_t *screenPtr = NULL;
uint32_t FRAMEBUFFER_HEIGHT = 0;
uint32_t FRAMEBUFFER_WIDTH = 0;

// Dangerus! may overflow
uint32_t *get_framebuffer_ptr() {
    return screenPtr;
}

// Check if the framebuffer is initialized
int is_framebuffer_inited() {
    return FRAMEBUFFER_WIDTH != 0;
}

// Set the pixel of the framebuffer
void framebuffer_set_pixel(uint32_t x, uint32_t y, uint32_t color) {
    screenPtr[y * FRAMEBUFFER_WIDTH + x] = color;
}

// Initialize the framebuffer
int framebuffer_init(multiboot_uint8_t *multibootInfo) {
    struct multiboot_tag_framebuffer *framebufferInfo = NULL;
    struct multiboot_tag *multibootTag = (struct multiboot_tag *) (multibootInfo + 8);

    if (multibootTag == NULL) { // No tag
        return -1;
    }

    // find multiboot framebuffer tag
    while (multibootTag->type != MULTIBOOT_TAG_TYPE_END) {
        if (multibootTag->type == MULTIBOOT_TAG_TYPE_FRAMEBUFFER) {
            framebufferInfo = ((struct multiboot_tag_framebuffer*)multibootTag);
            break;
        }

        multibootTag = (struct multiboot_tag *)(((multiboot_uint8_t *)(multibootTag)) + ((multibootTag->size + 7) & ~7));
    }

    if (framebufferInfo == NULL) {
        return -2;
    }

    screenPtr = (uint32_t*) framebufferInfo->common.framebuffer_addr;
    FRAMEBUFFER_HEIGHT = framebufferInfo->common.framebuffer_height;
    FRAMEBUFFER_WIDTH = framebufferInfo->common.framebuffer_width;

    return 0;
}
