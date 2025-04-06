#include <kernel/stdio.h>
#include <stdbool.h>

toSize_t printlogf(const char *format, ...) {
    toSize_t result = 0;
    __builtin_va_list args;
    __builtin_va_start(args, format);

    result += printlogf_nn(format, args);
    result += printf("\n");

    __builtin_va_end(args);

    return result;
}

toSize_t printlogf_nn(const char *format, ...) {
    toSize_t result = 0;
    __builtin_va_list args;
    __builtin_va_start(args, format);

    struct rtc_time time;
    get_rtc_time(&time);

    result += printf("[%d/%d/%d-%d:%d:%d] ", time.year, time.month, time.day, time.hour, time.minute, time.second);
    result += printf(format, args);

    __builtin_va_end(args);
    return result;
}

// Type flags
#define UNSIGNED_FLAG 0x01 // 00000001 unsigned
#define SIGNED_FLAG   0x02 // 00000010 signed
#define CLEAR_SIGNS_FLAG(flag) ((flag) &= 0xFC) // 11111100

#define INT_FLAG      0x04 // 00000100 int (or short)
#define LONG_FLAG     0x08 // 00001000 long
#define LONGLONG_FLAG 0x10 // 00010000 long long
#define CLEAR_TYPES_FLAG(flag) ((flag) &= 0xE3) // 11100011

#define INTEGER_FLAG  0x20 // 00100000 integer
#define FLOAT_FLAG    0x40 // 01000000 float-dot
#define CLEAR_DOTS_FLAG(flag) ((flag) &= 0x9F) // 10011111

// e.g.:
//     long double: LONG_FLAG | FLOAT_FLAG
//     double:      INT_FLAG | FLOAT_FLAG
//     int:         INT_FLAG | INTEGER_FLAG
//     long long:   LONGLONG_FLAG | INTEGER_FLAG

toSize_t printf(const char *format, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, format);

    char allParam[8192] = {0};
    unsigned alli = 0;
    for(int i = 0; format[i] != '\0'; ++i) {
        if(format[i] == '%') { // Insert param
            bool need_read = false;
            uint8_t type_flag = SIGNED_FLAG | INT_FLAG | INTEGER_FLAG; // signed int
            toSize_t float_digit = 0; // Float digit
            toSize_t default_float_digit = 3; // Default float digit
            do {
                need_read = false;
                switch(format[++i]) {
// Check Type flags
                    case 'u': { // unsigned param
                        CLEAR_SIGNS_FLAG(type_flag);
                        type_flag |= UNSIGNED_FLAG;
                        need_read = true;

                        break;
                    }

                    case 'l': { // long or long long param
                        if(type_flag & LONG_FLAG) { // Deal with long long
                            CLEAR_TYPES_FLAG(type_flag);
                            type_flag |= LONGLONG_FLAG;
                        } else {
                            CLEAR_TYPES_FLAG(type_flag);
                            type_flag |= LONG_FLAG;
                        }
                        need_read = true;

                        break;
                    }

                    case 'f': { // float-dot param
                        CLEAR_DOTS_FLAG(type_flag);
                        type_flag |= FLOAT_FLAG;
                        need_read = true;
                        while(format[i + 1] >= '0' && format[i + 1] <= '9') {
                            ++i;
                            float_digit *= 10;
                            float_digit += format[i] - '0';
                        }

                        break;
                    }

// Print mode(binary, decimal, hexadecimal, string, charater or "%%")
                    case 'b': { // Binary output mode
                        if(type_flag & SIGNED_FLAG) {
                            if(type_flag & INT_FLAG) {
                                if(type_flag & INTEGER_FLAG) { // signed int (binary)
                                    int pParam = __builtin_va_arg(args, int);
                                    if (pParam == 0) {
                                        allParam[alli++] = '0';
                                        break;
                                    }

                                    toSize_t start = alli;
                                    if (pParam < 0) {
                                        allParam[alli++] = '-';
                                        pParam = -pParam;
                                        start++;
                                    }

                                    toSize_t numStart = alli;
                                    do {
                                        allParam[alli++] = (pParam % 2) + '0';
                                        pParam /= 2;
                                    } while (pParam > 0);

                                    toSize_t left = numStart, right = alli - 1;
                                    while (left < right) {
                                        char tmp = allParam[left];
                                        allParam[left++] = allParam[right];
                                        allParam[right--] = tmp;
                                    }
                                } else if(type_flag & FLOAT_FLAG) { // double (binary)
                                    double dParam = __builtin_va_arg(args, double);
                                    if (dParam < 0) {
                                        allParam[alli++] = '-';
                                        dParam = -dParam;
                                    }

                                    long long intPart = (long long)dParam;
                                    double frac = dParam - intPart;

                                    // Deal with the integer part
                                    if (intPart == 0) {
                                        allParam[alli++] = '0';
                                    } else {
                                        toSize_t start = alli;
                                        while (intPart > 0) {
                                            allParam[alli++] = (intPart % 2) + '0';
                                            intPart /= 2;
                                        }

                                        toSize_t left = start, right = alli - 1;
                                        while (left < right) {
                                            char tmp = allParam[left];
                                            allParam[left++] = allParam[right];
                                            allParam[right--] = tmp;
                                        }
                                    }

                                    // Fractional part
                                    if (frac > 0) {
                                        allParam[alli++] = '.';
                                        toSize_t max_dot;
                                        if(float_digit == 0) { // Use default
                                            max_dot = default_float_digit;
                                        } else {
                                            max_dot = float_digit;
                                        }
                                        for (toSize_t i = 0; i < max_dot; ++i) {
                                            frac *= 2;
                                            int bit = (int)frac;
                                            allParam[alli++] = bit + '0';
                                            frac -= bit;
                                            if (frac == 0) {
                                                for(; i < max_dot - 1; allParam[alli++] = '0', ++i);
                                            }
                                        }
                                    }
                                } else {
                                    return 0;
                                }
                            } else if(type_flag & LONG_FLAG) {
                                if(type_flag & INTEGER_FLAG) { // signed long int (binary)
                                    long int pParam = __builtin_va_arg(args, long int);
                                    if (pParam == 0) {
                                        allParam[alli++] = '0';
                                        break;
                                    }

                                    toSize_t start = alli;
                                    if (pParam < 0) {
                                        allParam[alli++] = '-';
                                        pParam = -pParam;
                                        start++;
                                    }

                                    toSize_t numStart = alli;
                                    do {
                                        allParam[alli++] = (pParam % 2) + '0';
                                        pParam /= 2;
                                    } while (pParam > 0);

                                    toSize_t left = numStart, right = alli - 1;
                                    while (left < right) {
                                        char tmp = allParam[left];
                                        allParam[left++] = allParam[right];
                                        allParam[right--] = tmp;
                                    }
                                } else if(type_flag & FLOAT_FLAG) { // long double (binary)
                                    long double dParam = __builtin_va_arg(args, long double);
                                    if (dParam < 0) {
                                        allParam[alli++] = '-';
                                        dParam = -dParam;
                                    }

                                    long long intPart = (long long)dParam;
                                    double frac = dParam - intPart;

                                    // Deal with the integer part
                                    if (intPart == 0) {
                                        allParam[alli++] = '0';
                                    } else {
                                        toSize_t start = alli;
                                        while (intPart > 0) {
                                            allParam[alli++] = (intPart % 2) + '0';
                                            intPart /= 2;
                                        }

                                        toSize_t left = start, right = alli - 1;
                                        while (left < right) {
                                            char tmp = allParam[left];
                                            allParam[left++] = allParam[right];
                                            allParam[right--] = tmp;
                                        }
                                    }

                                    // Fractional part
                                    if (frac > 0) {
                                        allParam[alli++] = '.';
                                        toSize_t max_dot;
                                        if(float_digit == 0) { // Use default
                                            max_dot = default_float_digit;
                                        } else {
                                            max_dot = float_digit;
                                        }
                                        for (toSize_t i = 0; i < max_dot; ++i) {
                                            frac *= 2;
                                            int bit = (int)frac;
                                            allParam[alli++] = bit + '0';
                                            frac -= bit;
                                            if (frac == 0) {
                                                for(; i < max_dot - 1; allParam[alli++] = '0', ++i);
                                            }
                                        }
                                    }
                                } else {
                                    return 0;
                                }
                            } else if(type_flag & LONGLONG_FLAG) {
                                if(type_flag & INTEGER_FLAG) { // signed long long int (binary)
                                    long long int pParam = __builtin_va_arg(args, long long int);
                                    if (pParam == 0) {
                                        allParam[alli++] = '0';
                                        break;
                                    }

                                    toSize_t start = alli;
                                    if (pParam < 0) {
                                        allParam[alli++] = '-';
                                        pParam = -pParam;
                                        start++;
                                    }

                                    toSize_t numStart = alli;
                                    do {
                                        allParam[alli++] = (pParam % 2) + '0';
                                        pParam /= 2;
                                    } while (pParam > 0);

                                    toSize_t left = numStart, right = alli - 1;
                                    while (left < right) {
                                        char tmp = allParam[left];
                                        allParam[left++] = allParam[right];
                                        allParam[right--] = tmp;
                                    }
                                } else {
                                    return 0;
                                }
                            } else {
                                return 0;
                            }
                        } else if(type_flag & UNSIGNED_FLAG) {
                            if(type_flag & INT_FLAG) {
                                if(type_flag & INTEGER_FLAG) { // unsigned int (binary)
                                    unsigned int pParam = __builtin_va_arg(args, unsigned int);
                                    if (pParam == 0) {
                                        allParam[alli++] = '0';
                                        break;
                                    }

                                    toSize_t start = alli;
                                    do {
                                        allParam[alli++] = (pParam % 2) + '0';
                                        pParam /= 2;
                                    } while (pParam > 0);

                                    toSize_t left = start, right = alli - 1;
                                    while (left < right) {
                                        char tmp = allParam[left];
                                        allParam[left++] = allParam[right];
                                        allParam[right--] = tmp;
                                    }
                                } else {
                                    return 0;
                                }
                            } else if(type_flag & LONG_FLAG) {
                                if(type_flag & INTEGER_FLAG) { // unsigned long int (binary)
                                    unsigned long int pParam = __builtin_va_arg(args, unsigned long int);
                                    if (pParam == 0) {
                                        allParam[alli++] = '0';
                                        break;
                                    }

                                    toSize_t start = alli;
                                    do {
                                        allParam[alli++] = (pParam % 2) + '0';
                                        pParam /= 2;
                                    } while (pParam > 0);

                                    toSize_t left = start, right = alli - 1;
                                    while (left < right) {
                                        char tmp = allParam[left];
                                        allParam[left++] = allParam[right];
                                        allParam[right--] = tmp;
                                    }
                                } else {
                                    return 0;
                                }
                            } else if(type_flag & LONGLONG_FLAG) {
                                if(type_flag & INTEGER_FLAG) { // unsigned long long int (binary)
                                    unsigned long long int pParam = __builtin_va_arg(args, unsigned long long int);
                                    if (pParam == 0) {
                                        allParam[alli++] = '0';
                                        break;
                                    }

                                    toSize_t start = alli;
                                    do {
                                        allParam[alli++] = (pParam % 2) + '0';
                                        pParam /= 2;
                                    } while (pParam > 0);

                                    toSize_t left = start, right = alli - 1;
                                    while (left < right) {
                                        char tmp = allParam[left];
                                        allParam[left++] = allParam[right];
                                        allParam[right--] = tmp;
                                    }
                                } else {
                                    return 0;
                                }
                            } else {
                                return 0;
                            }
                        } else {
                            return 0;
                        }

                        break;
                    }

                    case 'd': { // Demical output mode
                        if (type_flag & SIGNED_FLAG) {
                            if (type_flag & INT_FLAG) {
                                if (type_flag & INTEGER_FLAG) { // signed int (binary)
                                    int pParam = __builtin_va_arg(args, int);
                                    if (pParam == 0) {
                                        allParam[alli++] = '0';
                                        break;
                                    }

                                    toSize_t start = alli;
                                    if (pParam < 0) {
                                        allParam[alli++] = '-';
                                        pParam = -pParam;
                                        start++;
                                    }

                                    toSize_t numStart = alli;
                                    do {
                                        allParam[alli++] = (pParam % 10) + '0';
                                        pParam /= 10;
                                    } while (pParam > 0);

                                    toSize_t left = numStart, right = alli - 1;
                                    while (left < right) {
                                        char tmp = allParam[left];
                                        allParam[left++] = allParam[right];
                                        allParam[right--] = tmp;
                                    }
                                } else if(type_flag & FLOAT_FLAG) { // double (binary)
                                    double dParam = __builtin_va_arg(args, double);
                                    if (dParam < 0) {
                                        allParam[alli++] = '-';
                                        dParam = -dParam;
                                    }

                                    long long intPart = (long long)dParam;
                                    double frac = dParam - intPart;

                                    // Deal with the integer part
                                    if (intPart == 0) {
                                        allParam[alli++] = '0';
                                    } else {
                                        toSize_t start = alli;
                                        while (intPart > 0) {
                                            allParam[alli++] = (intPart % 10) + '0';
                                            intPart /= 10;
                                        }
                                        toSize_t left = start, right = alli - 1;
                                        while (left < right) {
                                            char tmp = allParam[left];
                                            allParam[left++] = allParam[right];
                                            allParam[right--] = tmp;
                                        }
                                    }

                                    // Fractional part
                                    if (frac > 0) {
                                        allParam[alli++] = '.';
                                        toSize_t max_dot;
                                        if(float_digit == 0) { // Use default
                                            max_dot = default_float_digit;
                                        } else {
                                            max_dot = float_digit;
                                        }
                                        for (toSize_t i = 0; i < max_dot; i++) {
                                            frac *= 10;
                                            int digit = (int)frac;
                                            allParam[alli++] = digit + '0';
                                            frac -= digit;
                                            if (frac == 0) {
                                                for(; i < max_dot - 1; allParam[alli++] = '0', ++i);
                                            }
                                        }
                                    }
                                } else {
                                    return 0;
                                }
                            } else if(type_flag & LONG_FLAG) {
                                if(type_flag & INTEGER_FLAG) { // signed long int (binary)
                                    long int pParam = __builtin_va_arg(args, long int);
                                    if (pParam == 0) {
                                        allParam[alli++] = '0';
                                        break;
                                    }

                                    toSize_t start = alli;
                                    if (pParam < 0) {
                                        allParam[alli++] = '-';
                                        pParam = -pParam;
                                        start++;
                                    }

                                    toSize_t numStart = alli;
                                    do {
                                        allParam[alli++] = (pParam % 10) + '0';
                                        pParam /= 10;
                                    } while (pParam > 0);

                                    toSize_t left = numStart, right = alli - 1;
                                    while (left < right) {
                                        char tmp = allParam[left];
                                        allParam[left++] = allParam[right];
                                        allParam[right--] = tmp;
                                    }
                                } else if(type_flag & FLOAT_FLAG) { // long double (binary)
                                    long double dParam = __builtin_va_arg(args, long double);
                                    if (dParam < 0) {
                                        allParam[alli++] = '-';
                                        dParam = -dParam;
                                    }

                                    long long intPart = (long long)dParam;
                                    double frac = dParam - intPart;

                                    // Deal with the integer part
                                    if (intPart == 0) {
                                        allParam[alli++] = '0';
                                    } else {
                                        toSize_t start = alli;
                                        while (intPart > 0) {
                                            allParam[alli++] = (intPart % 10) + '0';
                                            intPart /= 10;
                                        }
                                        toSize_t left = start, right = alli - 1;
                                        while (left < right) {
                                            char tmp = allParam[left];
                                            allParam[left++] = allParam[right];
                                            allParam[right--] = tmp;
                                        }
                                    }

                                    // Fractional part
                                    if (frac > 0) {
                                        allParam[alli++] = '.';
                                        toSize_t max_dot;
                                        if(float_digit == 0) { // Use default
                                            max_dot = default_float_digit;
                                        } else {
                                            max_dot = float_digit;
                                        }
                                        for (toSize_t i = 0; i < max_dot; i++) {
                                            frac *= 10;
                                            int digit = (int)frac;
                                            allParam[alli++] = digit + '0';
                                            frac -= digit;
                                            if (frac == 0) {
                                                for(; i < max_dot - 1; allParam[alli++] = '0', ++i);
                                            }
                                        }
                                    }
                                } else {
                                    return 0;
                                }
                            } else if(type_flag & LONGLONG_FLAG) {
                                if(type_flag & INTEGER_FLAG) { // signed long long int (binary)
                                    long long int pParam = __builtin_va_arg(args, long long int);
                                    if (pParam == 0) {
                                        allParam[alli++] = '0';
                                        break;
                                    }

                                    toSize_t start = alli;
                                    if (pParam < 0) {
                                        allParam[alli++] = '-';
                                        pParam = -pParam;
                                        start++;
                                    }

                                    toSize_t numStart = alli;
                                    do {
                                        allParam[alli++] = (pParam % 10) + '0';
                                        pParam /= 10;
                                    } while (pParam > 0);

                                    toSize_t left = numStart, right = alli - 1;
                                    while (left < right) {
                                        char tmp = allParam[left];
                                        allParam[left++] = allParam[right];
                                        allParam[right--] = tmp;
                                    }
                                } else {
                                    return 0;
                                }
                            } else {
                                return 0;
                            }
                        } else if(type_flag & UNSIGNED_FLAG) {
                            if(type_flag & INT_FLAG) {
                                if(type_flag & INTEGER_FLAG) { // unsigned int (binary)
                                    unsigned int pParam = __builtin_va_arg(args, unsigned int);
                                    if (pParam == 0) {
                                        allParam[alli++] = '0';
                                        break;
                                    }

                                    toSize_t start = alli;
                                    do {
                                        allParam[alli++] = (pParam % 10) + '0';
                                        pParam /= 10;
                                    } while (pParam > 0);

                                    toSize_t left = start, right = alli - 1;
                                    while (left < right) {
                                        char tmp = allParam[left];
                                        allParam[left++] = allParam[right];
                                        allParam[right--] = tmp;
                                    }
                                } else {
                                    return 0;
                                }
                            } else if(type_flag & LONG_FLAG) {
                                if(type_flag & INTEGER_FLAG) { // unsigned long int (binary)
                                    unsigned long int pParam = __builtin_va_arg(args, unsigned long int);
                                    if (pParam == 0) {
                                        allParam[alli++] = '0';
                                        break;
                                    }

                                    toSize_t start = alli;
                                    do {
                                        allParam[alli++] = (pParam % 10) + '0';
                                        pParam /= 10;
                                    } while (pParam > 0);

                                    toSize_t left = start, right = alli - 1;
                                    while (left < right) {
                                        char tmp = allParam[left];
                                        allParam[left++] = allParam[right];
                                        allParam[right--] = tmp;
                                    }
                                } else {
                                    return 0;
                                }
                            } else if(type_flag & LONGLONG_FLAG) {
                                if(type_flag & INTEGER_FLAG) { // unsigned long long int (binary)
                                    unsigned long long int pParam = __builtin_va_arg(args, unsigned long long int);
                                    if (pParam == 0) {
                                        allParam[alli++] = '0';
                                        break;
                                    }

                                    toSize_t start = alli;
                                    do {
                                        allParam[alli++] = (pParam % 10) + '0';
                                        pParam /= 10;
                                    } while (pParam > 0);

                                    toSize_t left = start, right = alli - 1;
                                    while (left < right) {
                                        char tmp = allParam[left];
                                        allParam[left++] = allParam[right];
                                        allParam[right--] = tmp;
                                    }
                                } else {
                                    return 0;
                                }
                            } else {
                                return 0;
                            }
                        } else {
                            return 0;
                        }

                        break;
                    }

                    case 'x': { // Hexadecimal output mode
                        if(type_flag & SIGNED_FLAG) {
                            if(type_flag & INT_FLAG) {
                                if(type_flag & INTEGER_FLAG) { // signed int (binary)
                                    int pParam = __builtin_va_arg(args, int);
                                    if (pParam == 0) {
                                        allParam[alli++] = '0';
                                        break;
                                    }

                                    toSize_t start = alli;
                                    if (pParam < 0) {
                                        allParam[alli++] = '-';
                                        pParam = -pParam;
                                        start++;
                                    }

                                    toSize_t numStart = alli;
                                    do {
                                        int rem = pParam % 16;
                                        allParam[alli++] = rem < 10 ? rem + '0' : rem - 10 + 'A';
                                        pParam /= 16;
                                    } while (pParam > 0);

                                    toSize_t left = numStart, right = alli - 1;
                                    while (left < right) {
                                        char tmp = allParam[left];
                                        allParam[left++] = allParam[right];
                                        allParam[right--] = tmp;
                                    }
                                } else if(type_flag & FLOAT_FLAG) { // double (binary)
                                    double dParam = __builtin_va_arg(args, double);
                                    if (dParam < 0) {
                                        allParam[alli++] = '-';
                                        dParam = -dParam;
                                    }

                                    long long intPart = (long long)dParam;
                                    double frac = dParam - intPart;

                                    // Deal with the integer part
                                    if (intPart == 0) {
                                        allParam[alli++] = '0';
                                    } else {
                                        toSize_t start = alli;
                                        while (intPart > 0) {
                                            int rem = intPart % 16;
                                            allParam[alli++] = rem < 10 ? rem + '0' : rem - 10 + 'A';
                                            intPart /= 16;
                                        }
                                        toSize_t left = start, right = alli - 1;
                                        while (left < right) {
                                            char tmp = allParam[left];
                                            allParam[left++] = allParam[right];
                                            allParam[right--] = tmp;
                                        }
                                    }

                                    // Fractional part
                                    if (frac > 0) {
                                        allParam[alli++] = '.';
                                        toSize_t max_dot;
                                        if(float_digit == 0) { // Use default
                                            max_dot = default_float_digit;
                                        } else {
                                            max_dot = float_digit;
                                        }
                                        for (toSize_t i = 0; i < max_dot; i++) {
                                            frac *= 16;
                                            int digit = (int)frac;
                                            allParam[alli++] = digit < 10 ? digit + '0' : digit - 10 + 'A';
                                            frac -= digit;
                                            if (frac == 0) {
                                                for(; i < max_dot - 1; allParam[alli++] = '0', ++i);
                                            }
                                        }
                                    }
                                } else {
                                    return 0;
                                }
                            } else if(type_flag & LONG_FLAG) {
                                if(type_flag & INTEGER_FLAG) { // signed long int (binary)
                                    long int pParam = __builtin_va_arg(args, long int);
                                    if (pParam == 0) {
                                        allParam[alli++] = '0';
                                        break;
                                    }

                                    toSize_t start = alli;
                                    if (pParam < 0) {
                                        allParam[alli++] = '-';
                                        pParam = -pParam;
                                        start++;
                                    }

                                    toSize_t numStart = alli;
                                    do {
                                        int rem = pParam % 16;
                                        allParam[alli++] = rem < 10 ? rem + '0' : rem - 10 + 'A';
                                        pParam /= 16;
                                    } while (pParam > 0);

                                    toSize_t left = numStart, right = alli - 1;
                                    while (left < right) {
                                        char tmp = allParam[left];
                                        allParam[left++] = allParam[right];
                                        allParam[right--] = tmp;
                                    }
                                } else if(type_flag & FLOAT_FLAG) { // long double (binary)
                                    long double dParam = __builtin_va_arg(args, long double);
                                    if (dParam < 0) {
                                        allParam[alli++] = '-';
                                        dParam = -dParam;
                                    }

                                    long long intPart = (long long)dParam;
                                    double frac = dParam - intPart;

                                    // Deal with the integer part
                                    if (intPart == 0) {
                                        allParam[alli++] = '0';
                                    } else {
                                        toSize_t start = alli;
                                        while (intPart > 0) {
                                            int rem = intPart % 16;
                                            allParam[alli++] = rem < 10 ? rem + '0' : rem - 10 + 'A';
                                            intPart /= 16;
                                        }
                                        toSize_t left = start, right = alli - 1;
                                        while (left < right) {
                                            char tmp = allParam[left];
                                            allParam[left++] = allParam[right];
                                            allParam[right--] = tmp;
                                        }
                                    }

                                    // Fractional part
                                    if (frac > 0) {
                                        allParam[alli++] = '.';
                                        toSize_t max_dot;
                                        if(float_digit == 0) { // Use default
                                            max_dot = default_float_digit;
                                        } else {
                                            max_dot = float_digit;
                                        }
                                        for (toSize_t i = 0; i < max_dot; i++) {
                                            frac *= 16;
                                            int digit = (int)frac;
                                            allParam[alli++] = digit < 10 ? digit + '0' : digit - 10 + 'A';
                                            frac -= digit;
                                            if (frac == 0) {
                                                for(; i < max_dot - 1; allParam[alli++] = '0', ++i);
                                            }
                                        }
                                    }
                                } else {
                                    return 0;
                                }
                            } else if(type_flag & LONGLONG_FLAG) {
                                if(type_flag & INTEGER_FLAG) { // signed long long int (binary)
                                    long long int pParam = __builtin_va_arg(args, long long int);
                                    if (pParam == 0) {
                                        allParam[alli++] = '0';
                                        break;
                                    }

                                    toSize_t start = alli;
                                    if (pParam < 0) {
                                        allParam[alli++] = '-';
                                        pParam = -pParam;
                                        start++;
                                    }

                                    toSize_t numStart = alli;
                                    do {
                                        int rem = pParam % 16;
                                        allParam[alli++] = rem < 10 ? rem + '0' : rem - 10 + 'A';
                                        pParam /= 16;
                                    } while (pParam > 0);

                                    toSize_t left = numStart, right = alli - 1;
                                    while (left < right) {
                                        char tmp = allParam[left];
                                        allParam[left++] = allParam[right];
                                        allParam[right--] = tmp;
                                    }
                                } else {
                                    return 0;
                                }
                            } else {
                                return 0;
                            }
                        } else if(type_flag & UNSIGNED_FLAG) {
                            if(type_flag & INT_FLAG) {
                                if(type_flag & INTEGER_FLAG) { // unsigned int (binary)
                                    unsigned int pParam = __builtin_va_arg(args, unsigned int);
                                    if (pParam == 0) {
                                        allParam[alli++] = '0';
                                        break;
                                    }

                                    toSize_t start = alli;
                                    do {
                                        unsigned rem = pParam % 16;
                                        allParam[alli++] = rem < 10 ? rem + '0' : rem - 10 + 'A';
                                        pParam /= 16;
                                    } while (pParam > 0);

                                    toSize_t left = start, right = alli - 1;
                                    while (left < right) {
                                        char tmp = allParam[left];
                                        allParam[left++] = allParam[right];
                                        allParam[right--] = tmp;
                                    }
                                } else {
                                    return 0;
                                }
                            } else if(type_flag & LONG_FLAG) {
                                if(type_flag & INTEGER_FLAG) { // unsigned long int (binary)
                                    unsigned long int pParam = __builtin_va_arg(args, unsigned long int);
                                    if (pParam == 0) {
                                        allParam[alli++] = '0';
                                        break;
                                    }

                                    toSize_t start = alli;
                                    do {
                                        unsigned rem = pParam % 16;
                                        allParam[alli++] = rem < 10 ? rem + '0' : rem - 10 + 'A';
                                        pParam /= 16;
                                    } while (pParam > 0);

                                    toSize_t left = start, right = alli - 1;
                                    while (left < right) {
                                        char tmp = allParam[left];
                                        allParam[left++] = allParam[right];
                                        allParam[right--] = tmp;
                                    }
                                } else {
                                    return 0;
                                }
                            } else if(type_flag & LONGLONG_FLAG) {
                                if(type_flag & INTEGER_FLAG) { // unsigned long long int (binary)
                                    unsigned long long int pParam = __builtin_va_arg(args, unsigned long long int);
                                    if (pParam == 0) {
                                        allParam[alli++] = '0';
                                        break;
                                    }

                                    toSize_t start = alli;
                                    do {
                                        unsigned rem = pParam % 16;
                                        allParam[alli++] = rem < 10 ? rem + '0' : rem - 10 + 'A';
                                        pParam /= 16;
                                    } while (pParam > 0);

                                    toSize_t left = start, right = alli - 1;
                                    while (left < right) {
                                        char tmp = allParam[left];
                                        allParam[left++] = allParam[right];
                                        allParam[right--] = tmp;
                                    }
                                } else {
                                    return 0;
                                }
                            } else {
                                return 0;
                            }
                        } else {
                            return 0;
                        }

                        break;
                    }

                    case 's': { // String output mode
                        char *pParam = __builtin_va_arg(args, char *);
                        for(int k = 0; pParam[k] != '\0'; allParam[alli++] = pParam[k], ++k);
                        break;
                    }

                    case 'c': { // Character output mode
                        int pParam = __builtin_va_arg(args, int);
                        allParam[alli++] = pParam;
                        break;
                    }

                    case '%': { // "%%" output mode
                        allParam[alli++] = '%';
                        break;
                    }

                    default: { // other characters, we better will not get to here (
                        return 0;
                    }
                }
            } while(need_read);
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

    return alli;
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