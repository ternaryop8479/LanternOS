#include <kernel/io/serial.h>

#define COM1_PORT 0x3F8
#define SERIAL_TIMEOUT 1000000

static int serial_ready = 0;

// 端口输入
static uint8_t port_in(uint16_t port) {
    return inb(port);
}

// 端口输出
static void port_out(uint16_t port, uint8_t data) {
    outb(port, data);
}

int serial_init(void) {
    // 禁用中断
    port_out(COM1_PORT + 1, 0x00);
    
    // 设置波特率 (115200)
    port_out(COM1_PORT + 3, 0x80); // 启用DLAB
    port_out(COM1_PORT + 0, 0x01); // 分频低字节
    port_out(COM1_PORT + 1, 0x00); // 分频高字节
    
    // 8N1模式
    port_out(COM1_PORT + 3, 0x03);
    
    // 启用FIFO
    port_out(COM1_PORT + 2, 0xC7);
    
    // 硬件自检
    port_out(COM1_PORT + 4, 0x1E); // 环回模式
    port_out(COM1_PORT + 0, 0xAE); // 测试字节
    if(port_in(COM1_PORT + 0) != 0xAE) {
        return -1;
    }
    
    // 恢复正常模式
    port_out(COM1_PORT + 4, 0x0F);
    serial_ready = 1;
    return 0;
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
        // 处理换行符
        if (*p == '\n') serial_putc('\r');
    }
}

void serial_print(const char* str) {
    for (const char* p = str; *p; ++p) {
        serial_putc(*p);
        // 处理换行符
        if (*p == '\n') serial_putc('\r');
    }
}
