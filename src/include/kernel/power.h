#ifndef LanternOS_Header_KERNEL_POWER_H
#define LanternOS_Header_KERNEL_POWER_H

#include <kernel/types.h>

#define REBOOT_COLD 1
#define REBOOT_WARM 2

void powerinfo_init(EFI_SYSTEM_TABLE *systemTable);
bool poweroff();
bool reboot(int rebootMode);

#endif
