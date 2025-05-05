#ifndef LanternOS_Header_KERNEL_TIME_H
#define LanternOS_Header_KERNEL_TIME_H

#include <kernel/hal/rtc.h>

#define RTC_SECONDS 0x00
#define RTC_MINUTES 0x02
#define RTC_HOURS   0x04
#define RTC_DAYS    0x07
#define RTC_MONTHS  0x08
#define RTC_YEARS   0x09

struct rtc_time {
    uint32_t second;
    uint32_t minute;
    uint32_t hour;
    uint32_t day;
    uint32_t month;
    uint32_t year;
};

void get_rtc_time(struct rtc_time *time);
void sleep_for_ms(uint64_t millisecond);
void sleep_for_us(uint64_t microsecond);
void sleep_for_ns(uint64_t nanosecond);

#endif
