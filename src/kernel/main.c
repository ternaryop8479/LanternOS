#include <kernel/hal/framebuffer.h>
#include <kernel/types.h>
#include <kernel/hal/ports.h>
#include <kernel/terminal.h>
#include <kernel/time.h>
#include <kernel/power.h>
#include <kernel/memory_manager.h>
#include <kernel/vga/vga.h>
#include <kernel/format.h>
#include <kernel/hal/tsc.h>

EFI_STATUS EFIAPI kernel_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable) {
    // 初始化任务
    serial_init();
    printlogf("Kernel Start!");
    printlogf("Initializing framebuffer...");
    int result = framebuffer_init(systemTable);
    if(result != 0) {
        printlogf("Failed to initialize framebuffer! Code: %d", result);
        printlogf("Halt.");
        while(1) HALT();
    }
    vga_clear_screen(vga_background_color);
    printlogf("Initializing keyboard information...");
    keyboard_init();
    printlogf("Initializing power information...");
    powerinfo_init(systemTable);
    printlogf("Initializing TSC module...");
    tsc_init(systemTable, 256000ull); // 256 ms
    printlogf("Initializing memory manager...");
    EFI_STATUS status;
    status = mmanager_init(imageHandle, systemTable);
    if(EFI_ERROR(status)) {
        printlogf("Failed to initialize the memory map.");
        printlogf("Halt.");
        while(1) HALT();
    }
    printlogf("All initialization tasks done.");
    printlogf("Kernel successfully started!");

    // 开始正式执行内核代码
    sleep_for_ms(3000);
    vga_clear_screen(vga_background_color);
    printf("\nWelcome to Lantern OS (%s %s, %s build version)!\n", KERNEL_NAME, KERNEL_VERSION, KERNEL_DATE);
    printf("Copyright(C) Ternary_Operator.\n");
    printf("\n");
    const uint64_t MAX_CMD_SIZE = 256, MAX_PARAM_SIZE = 128;
    while(1) {
        printf("[Ternary_Operator: ~] $>. ");
        char commandline[MAX_CMD_SIZE], params[MAX_CMD_SIZE];
        memset(commandline, 0, MAX_CMD_SIZE);
        memset(params, 0, MAX_CMD_SIZE);
        getline(commandline); // Get commandline
        if(commandline[0] == '\0' || commandline[0] == '\n') {
            continue;
        }
        parse_commandline(commandline, params, MAX_CMD_SIZE);
        char exec[MAX_PARAM_SIZE];
        memset(exec, 0, MAX_PARAM_SIZE);
        parse_param(exec, params, 0);
        if(strcmp(exec, "free") == 0) {
            uint64_t totalSize = get_total_size(), freeSize = get_free_size();
            uint16_t totalUnit = size_unit(totalSize), freeUnit = size_unit(freeSize);
            printf("Total: %lf5d%s, Free: %lf5d%s\n", size_format(totalSize, totalUnit), unit_to_str(totalUnit), size_format(freeSize, freeUnit), unit_to_str(freeUnit));
        } else if(strcmp(exec, "poweroff") == 0) {
            printlogf("The system will poweroff(By user's operation).");
            poweroff();
        } else if(strcmp(exec, "reboot") == 0) {
            if(strcmp(getparam(params, 1, MAX_PARAM_SIZE), "cold") == 0) {
                printlogf("The system will reboot coldly(By user's operation).");
                reboot(REBOOT_COLD);
            } else if(strcmp(getparam(params, 1, MAX_PARAM_SIZE), "warm") == 0) {
                printlogf("The system will reboot warmly(By user's operation).");
                reboot(REBOOT_WARM);
            }
            printlogf("The system will reboot coldly(By user's operation).");
            reboot(REBOOT_COLD);
        } else if(strcmp(exec, "echo") == 0) {
            char param1[MAX_PARAM_SIZE];
            parse_param(param1, params, 1);
            printf("%s\n", param1);
        } else if(strcmp(exec, "exit") == 0) {
            printlogf("Exited.");
            return 0;
        } else if(strcmp(exec, "clear") == 0) {
            vga_clear_screen(terminal_background_color);
        } else if(strcmp(exec, "time") == 0) {
            struct rtc_time rtctime;
            get_rtc_time(&rtctime);
            printf("Current time is %d/%d/%d %d:%d:%d\n", rtctime.year, rtctime.month, rtctime.day, rtctime.hour, rtctime.minute, rtctime.second);
        } else {
            printf("Unknown command: %s\n", exec);
        }
        for(int i = 0; commandline[i] != '\0'; commandline[i++] = '\0');
    }
}
