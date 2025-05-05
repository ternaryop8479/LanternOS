#ifndef LanternOS_Header_KERNEL_FORMAT_H
#define LanternOS_Header_KERNEL_FORMAT_H

#include <kernel/types.h>

// Size formatters
#define SIZE_BYTE 0x0002
#define SIZE_KB 0x0003
#define SIZE_MB 0x0004
#define SIZE_GB 0x0005
#define SIZE_TB 0x0006
#define SIZE_PB 0x0007
#define SIZE_EB 0x0008
#define SIZE_ZB 0x0009
#define SIZE_YB 0x000A
uint16_t size_unit(uint64_t size);
long double size_format(uint64_t size, uint16_t format_unit);
const char *unit_to_str(uint16_t format_unit);

#endif
