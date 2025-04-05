#include <kernel/hal/keyboard.h>

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

static bool keyboard_initialized = false;

void hal_keyboard_init() {
    // Disable interruption
    asm volatile ("cli");
    
    // Send order to initialize keyboard
    while (inb(KEYBOARD_STATUS_PORT) & 0x02); // Wait for cleaning buffer
    outb(KEYBOARD_DATA_PORT, 0xF4); // Enable keyboard
    
    keyboard_initialized = true;
    
    // Enable interruption
    asm volatile ("sti");
}

int hal_keyboard_read_scancode() {
    if (!keyboard_initialized) return -1;
    
    if (inb(KEYBOARD_STATUS_PORT) & 0x01) {
        return inb(KEYBOARD_DATA_PORT);
    }
    return -1;
}
