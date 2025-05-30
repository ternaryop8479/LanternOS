#ifndef LanternOS_Header_KERNEL_IO_KEYBOARD_VIEWMAP_H
#define LanternOS_Header_KERNEL_IO_KEYBOARD_VIEWMAP_H

#include <kernel/types.h>

const char KEYMAP_NORMAL[128] = {
    0,   0,   '1', '2', '3', '4', '5', '6',  // 00-07
    '7', '8', '9', '0', '-', '=', '\b','\t', // 08-0F
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',  // 10-17
    'o', 'p', '[', ']', '\n',0,   'a', 's',  // 18-1F
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',  // 20-27
    '\'','`', 0,   '\\','z', 'x', 'c', 'v',  // 28-2F
    'b', 'n', 'm', ',', '.', '/', 0,   '*',  // 30-37
    0,   ' ', 0,   0,   0,   0,   0,   0,    // 38-3F
    0,   0,   0,   0,   0,   0,   0,   '7',  // 40-47
    '8', '9', '-', '4', '5', '6', '+', '1',  // 48-4F
    '2', '3', '0', '.', 0,   0,   0,   0    // 50-57
};

const char KEYMAP_SHIFTED[128] = {
    0,   0,   '!', '@', '#', '$', '%', '^',  // 00-07
    '&', '*', '(', ')', '_', '+', '\b','\t', // 08-0F
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',  // 10-17
    'O', 'P', '{', '}', '\n',0,   'A', 'S',  // 18-1F
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',  // 20-27
    '"', '~', 0,   '|', 'Z', 'X', 'C', 'V',  // 28-2F
    'B', 'N', 'M', '<', '>', '?', 0,   '*',  // 30-37
    0,   ' ', 0,   0,   0,   0,   0,   0,    // 38-3F
    0,   0,   0,   0,   0,   0,   0,   '7',  // 40-47
    '8', '9', '-', '4', '5', '6', '+', '1',  // 48-4F
    '2', '3', '0', '.', 0,   0,   0,   0    // 50-57
};

#endif
