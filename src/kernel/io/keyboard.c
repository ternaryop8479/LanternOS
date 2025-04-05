#include <kernel/io/keyboard.h>
#include <kernel/io/keyboard_viewmap.h>

static bool shift_pressed = false;
static bool caps_lock = false;

void keyboard_init() {
    hal_keyboard_init();
}

char scancode_to_char(uint8_t scancode) {
    // Release code
    if (scancode & 0x80) {
        // Release code of LShift and RShift
        if(scancode == 0xAA || scancode == 0xB6) {
            shift_pressed = false;
        }
        return 0;
    }

    // Special keys
    switch(scancode) {
        case 0x2A: // Left Shift
            shift_pressed = true;
            return 0;
        case 0x36: // Right Shift
            shift_pressed = true;
            return 0;
        case 0x3A: // Caps Lock
            caps_lock = !caps_lock;
            return 0;
    }

    // Translate to ASCII
    char result;
    if(shift_pressed) {
        result = KEYMAP_SHIFTED[scancode];
    } else if(caps_lock) {
        if(KEYMAP_NORMAL[scancode] >= 'a' && KEYMAP_NORMAL[scancode] <= 'z') {
            result = KEYMAP_SHIFTED[scancode];
        } else {
            result = KEYMAP_NORMAL[scancode];
        }
    } else {
        result = KEYMAP_NORMAL[scancode];
    }

    return result;
}

void checkch(char *map) {
    int scancode;
    int index = 0;
    
    while ((scancode = hal_keyboard_read_scancode()) != -1) {
        char ch = scancode_to_char(scancode);
        if (ch != 0) {
            map[index++] = ch;
        }
    }
    map[index] = '\0';
}
