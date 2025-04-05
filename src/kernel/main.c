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
    while(1) {
        printf("[Ternary_Operator: ~] $>. ");
        char command[4096];
        getline(command);
        if(command[0] == 'e' && command[1] == 'x' && command[2] == 'i' && command[3] == 't') {
            printlogf("Exited.");
            return;
        } else if(command[0] == 'c' && command[1] == 'l' && command[2] == 'e' && command[3] == 'a' && command[4] == 'r') {
            vga_clear_screen(terminal_background_color);
        } else if(command[0] == 't' && command[1] == 'i' && command[2] == 'm' && command[3] == 'e') {
            struct rtc_time rtctime;
            get_rtc_time(&rtctime);
            printf("Current time is %d/%d/%d %d:%d:%d\n", rtctime.year, rtctime.month, rtctime.day, rtctime.hour, rtctime.minute, rtctime.second);
        } else {
            printf("Unknown command: %s\n", command);
        }
        for(int i = 0; command[i] != '\0'; command[i++] = '\0');
    }
}
