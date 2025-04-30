#include <kernel/hal/framebuffer.h>
#include <kernel/types.h>
#include <kernel/hal/ports.h>
#include <kernel/terminal.h>
#include <kernel/time.h>
#include <kernel/power.h>

EFI_STATUS EFIAPI kernel_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable) {
    // 初始化任务
    serial_init();
    printlogf("Kernel Start!");
    printlogf("Initializing keyboard information...");
    keyboard_init(systemTable);
    printlogf("Initializing power information...");
    powerinfo_init(systemTable);
    printlogf("Initializing framebuffer...");
    int result = framebuffer_init(systemTable);
    if(result != 0) {
        printlogf("Failed to initialize framebuffer! Code: %d", result);
        printlogf("Halt.");
        while(1) HALT();
    }
    printlogf("All initialization tasks done.");
    printlogf("Kernel successfully started!");

    // 开始正式执行内核代码
    vga_clear_screen(vga_background_color);
    printf("\nWelcome to Lantern OS (%s %s, %s build version)!\n", KERNEL_NAME, KERNEL_VERSION, KERNEL_DATE);
    printf("Copyright(C) Ternary_Operator.\n");
    printf("\n");
    printlogf("This is a log out(EXCEPTED 0x5201F): %ulx", 0x5201Ful);
    const uint64_t MAX_CMD_SIZE = 256, MAX_PARAM_SIZE = 128;
    while(1) {
        printf("[Ternary_Operator: ~] $>. ");
        char commandline[MAX_CMD_SIZE], params[MAX_CMD_SIZE];
        getline(commandline); // Get commandline
        parse_commandline(commandline, params, MAX_CMD_SIZE);
        char exec[MAX_PARAM_SIZE];
        parse_param(exec, params, 0);
        if(strcmp(exec, "poweroff") == 0) {
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
