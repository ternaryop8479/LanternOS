#include <kernel/hal/rtc.h>

uint8_t read_RTC(uint8_t reg) {
    outb(RTC_INDEX_PORT, reg);
    io_wait();
    return inb(RTC_DATA_PORT);
}
