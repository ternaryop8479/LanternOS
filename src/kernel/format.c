#include <kernel/format.h>

uint16_t size_unit(uint64_t size) {
    uint16_t result = SIZE_BYTE;
    for(; size > 1024; size /= 1024, ++result);
    return result;
}

long double size_format(uint64_t size, uint16_t format_unit) {
    long double divNum = 1.0;
    for(uint64_t i = 0; i < format_unit - SIZE_BYTE; divNum *= 1024, ++i);
    return size / divNum;
}

const char *unit_to_str(uint16_t format_unit) {
    switch(format_unit) {
        case SIZE_BYTE:
            return "Byte";

        case SIZE_KB:
            return "KB";

        case SIZE_MB:
            return "MB";

        case SIZE_GB:
            return "GB";

        case SIZE_TB:
            return "TB";

        case SIZE_PB:
            return "PB";

        case SIZE_EB:
            return "EB";

        case SIZE_ZB:
            return "ZB";

        case SIZE_YB:
            return "YB";

        default:
            return "UN";
    }
}
