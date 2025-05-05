#include <kernel/time.h>
#include <kernel/hal/tsc.h>

static uint32_t bcd_to_decimal(uint8_t value) {
    return ((value >> 4) * 10) + (value & 0x0F);
}

void get_rtc_time(struct rtc_time *time) {
    uint8_t reg_b = read_RTC(0x0B); // Read RTC settings from register
    
    time->second = (read_RTC(RTC_SECONDS));
    time->minute = (read_RTC(RTC_MINUTES));
    time->hour   = (read_RTC(RTC_HOURS));
    time->day    = (read_RTC(RTC_DAYS));
    time->month  = (read_RTC(RTC_MONTHS));
    time->year   = (read_RTC(RTC_YEARS));

    // Translate BCD format to decimal format
    if (!(reg_b & 0x04)) {
        time->second = bcd_to_decimal(time->second);
        time->minute = bcd_to_decimal(time->minute);
        time->hour   = bcd_to_decimal(time->hour);
        time->day    = bcd_to_decimal(time->day);
        time->month  = bcd_to_decimal(time->month);
        time->year   = bcd_to_decimal(time->year);
    }

    // Process with 12-hour time
    if (!(reg_b & 0x02) && (time->hour & 0x80)) {
        time->hour = ((time->hour & 0x7F) + 12) % 24;
    }

    time->year += 2000;
}

void sleep_for_ms(uint64_t millisecond) {
    sleep_for_ns(millisecond * 1000 * 1000);
}
void sleep_for_us(uint64_t microsecond) {
    sleep_for_ns(microsecond * 1000);
}
void sleep_for_ns(uint64_t nanosecond) {
    uint64_t startTSC = read_tsc();
    while(tsc_to_nanosecond(read_tsc() - startTSC) < nanosecond) {
        asm volatile ("pause");
    }
}
