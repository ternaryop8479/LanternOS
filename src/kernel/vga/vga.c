#include <kernel/vga/vga.h>
#include <kernel/vga/font_8x16.h>

static uint32_t screenPtr_X = 0, screenPtr_Y = 0;
static uint32_t font_width = 8, font_height = 16;

uint32_t vga_background_color = 0, vga_foreground_color = 0xFFFFFF;

// Clear the current line
static int vga_clear_line() {
    int result = 0;
    for(uint32_t y = screenPtr_Y; y <= screenPtr_Y + font_height; ++y) { // Clear line
        for(uint32_t x = 0; x < VGA_WIDTH; ++x) {
            result += vga_set_pixel(x, y, vga_background_color);
        }
    }
    screenPtr_X = 0;
    return result;
}

int vga_move_cursor(uint8_t order) {
    if(order == VGA_UPCHAR) {
        screenPtr_Y -= font_height;
        if((int32_t)screenPtr_Y < 0) {
            screenPtr_Y = 0;
            return 0;
        }
    } else if(order == VGA_DOWNCHAR) {
        screenPtr_Y += font_height;
        if(screenPtr_Y >= VGA_HEIGHT) {
            screenPtr_Y -= font_height;
            return 0;
        }
    } else if(order == VGA_LEFTCHAR) {
        screenPtr_X -= font_width;
        if((int32_t)screenPtr_X < 0) {
            screenPtr_X = 0;
            return 0;
        }
    } else if(order == VGA_RIGHTCHAR) {
        screenPtr_X += font_width;
        if(screenPtr_X >= VGA_WIDTH) {
            screenPtr_X -= font_width;
            return 0;
        }
    }
    return 1;
}

void vga_backspace() {
    if(!vga_move_cursor(VGA_LEFTCHAR)) {
        if(vga_move_cursor(VGA_UPCHAR)) {
            while(vga_move_cursor(VGA_RIGHTCHAR));
        }
    }
    vga_putc(' ');
    vga_move_cursor(VGA_LEFTCHAR);
}

int vga_set_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if(!is_framebuffer_inited()) {
        return -1;
    }
    if(x >= VGA_WIDTH || y >= VGA_HEIGHT) {
        return -2;
    }
    framebuffer_set_pixel(x, y, color);
    return 0;
}

int vga_clear_screen(uint32_t color) {
    if(!is_framebuffer_inited()) { // Optimize by framebuffer_set_pixel() instead of vga_set_pixel()
        return -1;
    }
    for(uint32_t y = 0; y < VGA_HEIGHT; ++y) {
        for(uint32_t x = 0; x < VGA_WIDTH; ++x) {
            framebuffer_set_pixel(x, y, color);
        }
    }
    screenPtr_X = 0, screenPtr_Y = 0;
    return 0;
}

int vga_putc(char c) {
    int isEnter = 0;
    // Deal with '\n' and '\r'
    if(c == '\n') {
        isEnter = 1;
    } else if(c == '\r') {
        screenPtr_X = 0;
        vga_clear_line();
        return 0;
    }

    // Calculate offeset of the font data
    uint32_t char_index = (uint32_t)c * font_height;

    // Calculate real height of we need
    uint32_t REAL_HEIGHT = VGA_HEIGHT - (VGA_HEIGHT % font_height);

    // Check if need process enter
    if(screenPtr_X + font_width > VGA_WIDTH || isEnter) {
        screenPtr_X = 0;
        screenPtr_Y += font_height;
        // Check if need scroll the screen
        if(screenPtr_Y >= REAL_HEIGHT) {
            screenPtr_Y = REAL_HEIGHT - font_height; // Update the screenPtr_Y
            uint32_t *fb_ptr = get_framebuffer_ptr(); // Scroll the screen
            // Copy the source frame to target frame
            memcpy(fb_ptr, fb_ptr + (font_height * VGA_WIDTH), (REAL_HEIGHT - font_height) * VGA_WIDTH * sizeof(uint32_t));
            vga_clear_line();
        }
        if(isEnter) {
            return 0;
        }
    }

    // Print character
    for(uint32_t y = 0; y < font_height; ++y) {
        uint8_t byte = font_8x16_data[char_index + y];
        for(uint32_t x = 0; x < font_width; ++x) {
            if(byte & (0x80 >> x)) { // If this bit is 1
                vga_set_pixel(screenPtr_X + x, screenPtr_Y + y, vga_foreground_color); // Use foreground color
            } else {
                vga_set_pixel(screenPtr_X + x, screenPtr_Y + y, vga_background_color);
            }
        }
    }

    // Update screen pointer(seek)
    screenPtr_X += font_width;

    return 0;
}

// Print string to the VGA
int vga_putstr(const char *str, uint64_t strSize) {
    int result = 0;
    for(uint64_t i = 0; i < strSize; ++i) {
        result += vga_putc(str[i]);
    }
    return result;
}
