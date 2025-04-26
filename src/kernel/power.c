#include <kernel/power.h>

static EFI_SYSTEM_TABLE *SystemTable = NULL;

void powerinfo_init(EFI_SYSTEM_TABLE *systemTable) {
    SystemTable = systemTable;
}

bool poweroff() {
    if(SystemTable == NULL) {
        return false;
    }
    SystemTable->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
    return true;
}

bool reboot(int rebootMode) {
    if(SystemTable == NULL || (rebootMode != REBOOT_WARM && rebootMode != REBOOT_COLD)) {
        return false;
    }
    SystemTable->RuntimeServices->ResetSystem(rebootMode == REBOOT_WARM ? EfiResetWarm : EfiResetCold, EFI_SUCCESS, 0, NULL);
    return true;
}
