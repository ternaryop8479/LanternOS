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


// static char normal_chars[] = {'a', 'b', 'f', 'n', 'r', 't', 'v', '\\', '\'', '\"', '?'};
// static char escape_chars[] = {'\a', '\b', '\f', '\n', '\r', '\t', '\v', '\\', '\'', '\"', '\?'};

// static short is_chneed_escaped(char target) {
//     short res = 0;
//     for(int i = 0; i < sizeof(normal_chars); (target == normal_chars[i]) ? res = 1 : 0, ++i);
//     return res;
// }

// static char cescape_to_normal(char ch) {
//     char res = 0;
//     for(int i = 0; i < sizeof(escape_chars); (ch == escape_chars[i]) ? res =  : 0, ++i);
//     return res;
// }

// void escape_to_normal(char *normal_str, const char *escape_str, uint64_t max_buffer_size) {
    
// }

// void normal_to_escape(char *escape_str, const char *normal_str, uint64_t max_buffer_size) {
    
// }

// void parse_commandline(const char *commandline, char *params, uint64_t max_buffer_size) {
//     for(uint64_t i = 0, parami = 0; commandline[i] != '\0'; ++i) {
//         if(commandline[i] == '\"') {
//             uint64_t nowParamI = parami; // Record the offeset when starting parsing
//             while(commandline[++i] != '\"') { // Until find the matched \"
//                 if(parami + 1 < max_buffer_size) {
//                     if(is_chneed_escaped(commandline[i])) {
//                         params[parami++] = cescape_to_normal(commandline[i]);
//                     }
//                     params[parami++] = commandline[i];
//                 } else {
//                     return;
//                 }
//             }
//             params[parami++] = '\n';
//         }
//     }
// }
