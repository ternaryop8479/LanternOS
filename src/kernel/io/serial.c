#include <kernel/io/serial.h>
#include <kernel/types.h>

#define COM1_PORT 0x3F8
#define SERIAL_TIMEOUT 1000000

static int serial_ready = 0;

static uint8_t port_in(uint16_t port) {
    return inb(port);
}

static void port_out(uint16_t port, uint8_t data) {
    outb(port, data);
}

KERNEL_STATUS serial_init(void) {
    // Disable interrupt
    port_out(COM1_PORT + 1, 0x00);

    // Set baud rate (115200)
    port_out(COM1_PORT + 3, 0x80); // Enable DLAB
    port_out(COM1_PORT + 0, 0x01); // Low-bytes
    port_out(COM1_PORT + 1, 0x00); // High-bytes

    // 8N1 Mode
    port_out(COM1_PORT + 3, 0x03);

    // Enable FIFO
    port_out(COM1_PORT + 2, 0xC7);

    // Hardware self-check
    port_out(COM1_PORT + 4, 0x1E); // Loop test
    port_out(COM1_PORT + 0, 0xAE); // Test bytes
    if(port_in(COM1_PORT + 0) != 0xAE) {
        return KERNEL_ERROR_FAILED_SELFCHECK;
    }

    // Restore normal mode
    port_out(COM1_PORT + 4, 0x0F);
    serial_ready = 1;
    return KERNEL_SUCCESS;
}

void serial_putc(char c) {
    if (!serial_ready) return;
    
    int timeout = SERIAL_TIMEOUT;
    while ((port_in(COM1_PORT + 5) & 0x20) == 0) {
        if (--timeout == 0) return;
    }
    port_out(COM1_PORT, (uint8_t)c);
}

void serial_write(const char *data, uint64_t dataSize) {
    for (const char* p = data; (uint64_t)(p - data) < dataSize; ++p) {
        serial_putc(*p);
        if (*p == '\n') serial_putc('\r');
    }
}

void serial_print(const char* str) {
    for (const char* p = str; *p; ++p) {
        serial_putc(*p);
        if (*p == '\n') serial_putc('\r');
    }
}
