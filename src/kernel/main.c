#include <boot/init.h>
#include <kernel/terminal.h>
#include <kernel/time.h>

void kernel_main(unsigned int magic, multiboot_uint8_t *multiboot_header) {
    // 初始化任务
    serial_init();
    printlogf("Kernel Start!");
    printlogf("Checking multiboot2 magic number...");
    if(magic != MULTIBOOT2_BOOTLOADER_MAGIC) { // 检查魔数
        printlogf("[error] Magic number error: received %x, expected %x. Halt.", magic, MULTIBOOT2_BOOTLOADER_MAGIC);
        while(1) HALT();
    }
    printlogf("Initializing boot page table...");
    init_boot_pagetable(); // 页表初始化
    printlogf("Decoding and loading framebuffer...");
    if(framebuffer_init(multiboot_header) != 0) {
        printlogf("[error] Failed to initalize framebuffer! Halt.");
        while(1) HALT();
    }
    printlogf("Initializing keyboard...");
    keyboard_init();
    printlogf("All initialization tasks done.");
    printlogf("Kernel successfully started!");

    // 开始正式执行内核代码
    vga_clear_screen(vga_background_color);
    printf("\nWelcome to Lantern OS (%s %s, %s build version)!\n", KERNEL_NAME, KERNEL_VERSION, KERNEL_DATE);
    printf("Copyright(C) Ternary_Operator.\n");
    printf("\n");
    const uint64_t MAX_CMD_SIZE = 1024, MAX_PARAM_SIZE = 512;
    while(1) {
        printf("[Ternary_Operator: ~] $>. ");
        char commandline[MAX_CMD_SIZE], params[MAX_CMD_SIZE];
        getline(commandline); // Get commandline
        parse_commandline(commandline, params, MAX_CMD_SIZE);
        char exec[MAX_PARAM_SIZE];
        parse_param(exec, params, 0);
        if(strcmp(exec, "echo") == 0) {
            char param1[MAX_PARAM_SIZE];
            parse_param(param1, params, 1);
            printf("%s\n", param1);
        } else if(strcmp(exec, "exit") == 0) {
            printlogf("Exited.");
            return;
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
