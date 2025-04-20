#ifndef LanternOS_Header_KERNEL_STRING_H
#define LanternOS_Header_KERNEL_STRING_H

#include <kernel/types.h>

void *memcpy(void *dest, const void *src, size_t n);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, uint64_t n);

#endif
