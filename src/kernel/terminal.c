#include <kernel/terminal.h>

uint32_t terminal_cursor_color     = 0xFFFFFF;
uint32_t terminal_background_color = 0x000000;
uint32_t terminal_foreground_color = 0xFFFFFF;

void draw_cursor() {
    vga_background_color = terminal_cursor_color;
    vga_putc(' ');
    vga_backspace();
    vga_background_color = terminal_background_color;
}

void getline(char *buffer) {
    draw_cursor();
    uint64_t offeset = 0;
    while(1) {
        char ch = getch();
        vga_background_color = terminal_background_color, vga_foreground_color = terminal_foreground_color;
        if(ch == '\n' || ch == '\0') {
            vga_putc(' ');
            vga_backspace();
            vga_putc('\n');
            return;
        } else if(ch == '\b') {
            if(offeset == 0) {
                continue;
            }
            --offeset;
            buffer[offeset] = '\0';
            vga_putc(' ');
            vga_backspace();
            vga_backspace();
        } else {
            buffer[offeset++] = ch;
            vga_putc(ch);
        }
        draw_cursor();
    }
    buffer[offeset] = '\0';
}
