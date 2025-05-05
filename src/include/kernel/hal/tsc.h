#ifndef LanternOS_Header_KERNEL_HAL_TSC_H
#define LanternOS_Header_KERNEL_HAL_TSC_H

#include <kernel/types.h>

KERNEL_STATUS tsc_init(EFI_SYSTEM_TABLE *systemTable, uint64_t calibrate_microseconds);
bool check_invariant_tsc();
uint64_t read_tsc();
uint64_t tsc_to_nanosecond(uint64_t tsc);

#endif
