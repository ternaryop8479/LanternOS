#ifndef LanternOS_Header_KERNEL_HAL_RTC_H
#define LanternOS_Header_KERNEL_HAL_RTC_H

#include <kernel/types.h>
#include <kernel/hal/ports.h>

#define RTC_INDEX_PORT 0x70
#define RTC_DATA_PORT 0x71

uint8_t read_RTC(uint8_t reg);

#endif
