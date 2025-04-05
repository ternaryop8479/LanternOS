#include <kernel/time.h>

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
