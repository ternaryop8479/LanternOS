#include <kernel/stdio.h>

typedef uint64_t toSize_t;

void printlogf(const char *format, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, format);

    printlogf_nn(format, args);
    printf("\n");

    __builtin_va_end(args);
}

void printlogf_nn(const char *format, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, format);

    struct rtc_time time;
    get_rtc_time(&time);

    printf("[%d/%d/%d-%d:%d:%d] ", time.year, time.month, time.day, time.hour, time.minute, time.second);
    printf(format, args);

    __builtin_va_end(args);
}

void printf(const char *format, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, format);

    char allParam[8192] = {0};
    unsigned alli = 0;
    for(int i = 0; format[i] != '\0'; ++i) {
        if(format[i] == '%') { // Insert param
            switch(format[++i]) {
                case 'd': { // Integer param
                    int pParam = __builtin_va_arg(args, int);
                    if(pParam == 0) {
                        allParam[alli++] = '0';
                        continue;
                    } else if(pParam < 0) {
                        allParam[alli++] = '-';
                        pParam = -pParam;
                    }

                    toSize_t alliST = alli;

                    while(pParam != 0) {
                        int rem = pParam % 10;
                        allParam[alli++] = rem ^ 48;
                        pParam /= 10;
                    }

                    toSize_t left = alliST, right = alli - 1;
                    while(left < right) {
                        char temp = allParam[left];
                        allParam[left] = allParam[right];
                        allParam[right] = temp;
                        ++left;
                        --right;
                    }

                    break;
                }

                case 'x': { // Hex param
                    int pParam = __builtin_va_arg(args, int);
                    if (pParam == 0) {
                        allParam[alli++] = '0';
                        continue;
                    } else if (pParam < 0) {
                        allParam[alli++] = '-';
                        pParam = -pParam;
                    }

                    toSize_t alliST = alli;

                    while (pParam != 0) {
                        int rem = pParam % 16;
                        if (rem < 10) {
                            allParam[alli++] = rem ^ 48; // Character 0-9
                        } else {
                            allParam[alli++] = rem - 10 + 'A'; // Character A-F
                        }
                        pParam /= 16;
                    }

                    toSize_t left = alliST, right = alli - 1;
                    while (left < right) {
                        char temp = allParam[left];
                        allParam[left] = allParam[right];
                        allParam[right] = temp;
                        ++left;
                        --right;
                    }

                    break;
                }

                case 'f': {
                    double pParam = __builtin_va_arg(args, double);
                    short digit;
                    ++i;
                    if(format[i] >= '0' && format[i] <= '9') {
                        digit = format[i] ^ 48;
                    } else {
                        --i;
                        break;
                    }

                    if (pParam < 0) {
                        allParam[alli++] = '-';
                        pParam = -pParam;
                    }

                    // Infinity of NaN
                    if (pParam == -pParam) {
                        if (pParam == 0) {
                            allParam[alli++] = '0', allParam[alli++] = '.';
                            for(short k = 1; k <= digit; allParam[alli++] = '0', ++k);
                            ++i;
                            continue;
                        } else {
                            allParam[alli++] = 'N', allParam[alli++] = 'a', allParam[alli++] = 'N';
                            continue;
                        }
                    }

                    long inParam = pParam;

                    toSize_t alliST = alli;

                    while(inParam != 0) {
                        int rem = inParam % 10;
                        allParam[alli++] = rem ^ 48;
                        inParam /= 10;
                    }

                    toSize_t left = alliST, right = alli - 1;
                    while(left < right) {
                        char temp = allParam[left];
                        allParam[left] = allParam[right];
                        allParam[right] = temp;
                        ++left;
                        --right;
                    }

                    allParam[alli++] = '.';

                    alliST = alli;

                    long pls = 1;
                    for(int k = 1; k <= digit; pls *= 10, ++k);

                    pParam -= (int)(pParam);

                    long digitalParam = pParam * pls;
                    ++digitalParam; // Fix the mistake of the float-dot calculation

                    while(digitalParam != 0) {
                        int rem = digitalParam % 10;
                        allParam[alli++] = rem ^ 48;
                        digitalParam /= 10;
                    }

                    left = alliST, right = alli - 1;
                    while(left < right) {
                        char temp = allParam[left];
                        allParam[left] = allParam[right];
                        allParam[right] = temp;
                        ++left;
                        --right;
                    }

                    break;
                }

                case 's': {
                    char *pParam = __builtin_va_arg(args, char *);
                    for(int k = 0; pParam[k] != '\0'; allParam[alli++] = pParam[k], ++k);
                    break;
                }

                case 'c': {
                    int pParam = __builtin_va_arg(args, int);
                    allParam[alli++] = pParam;
                    break;
                }

                case '%': {
                    allParam[alli++] = '%';
                }

                default: {
                    break;
                }
            }
        } else {
            allParam[alli++] = format[i];
        }
    }

    __builtin_va_end(args);

    // Put string
    if(is_framebuffer_inited()) {
        vga_putstr(allParam, alli);
    } else {
        serial_write(allParam, alli);
    }
}

char getch() {
    char map[256];
    while(1) {
        checkch(map);
        if(map[0] != '\0') {
            char first = map[0];
            for(int i=0; map[i]; i++) {
                map[i] = map[i+1];
            }
            return first;
        }
        io_wait();
    }
}
