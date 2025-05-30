#include <kernel/string.h>
#include <kernel/memory_manager.h>

void *memcpy(void *dest, const void *src, uint64_t n) {
    char *d = (char *)dest;
    const char *s = (const char *)src;
    const size_t align = sizeof(unsigned long);

    // Copy by bytes to align
    while (n > 0 && ((uintptr_t)d % align != 0 || (uintptr_t)s % align != 0)) {
        *d++ = *s++;
        n--;
    }

    if (n == 0) {
        return dest;
    }

    // Copy by pages
    const size_t page_size = PAGE_SIZE;
    size_t page_count = n / page_size;
    size_t words_per_page = page_size / align;

    for (size_t i = 0; i < page_count; ++i) {
        unsigned long *d_word = (unsigned long *)d;
        const unsigned long *s_word = (const unsigned long *)s;
        for (size_t j = 0; j < words_per_page; ++j) {
            *d_word++ = *s_word++;
        }
        d += page_size;
        s += page_size;
        n -= page_size;
    }

    // Copy by word
    size_t word_count = n / align;
    unsigned long *d_word = (unsigned long *)d;
    const unsigned long *s_word = (const unsigned long *)s;
    for (size_t i = 0; i < word_count; ++i) {
        *d_word++ = *s_word++;
    }
    d = (char *)d_word;
    s = (const char *)s_word;
    n %= align;

    // Copy the latest bytes
    while (n--) {
        *d++ = *s++;
    }

    return dest;
}

void *memset(void *dest, char ch, uint64_t n) {
    char *d = (char *)dest;
    const size_t align = sizeof(unsigned long);

    // Copy by bytes to align
    while (n > 0 && (uintptr_t)d % align != 0) {
        *d++ = ch;
        n--;
    }

    if (n == 0) {
        return dest;
    }

    // Copy by pages
    const size_t page_size = PAGE_SIZE;
    size_t page_count = n / page_size;
    size_t words_per_page = page_size / align;

    for (size_t i = 0; i < page_count; ++i) {
        unsigned long *d_word = (unsigned long *)d;
        for (size_t j = 0; j < words_per_page; ++j) {
            *d_word++ = ch;
        }
        d += page_size;
        n -= page_size;
    }

    // Copy by word
    size_t word_count = n / align;
    unsigned long *d_word = (unsigned long *)d;
    for (size_t i = 0; i < word_count; ++i) {
        *d_word++ = ch;
    }
    d = (char *)d_word;
    n %= align;

    // Copy the latest bytes
    while (n--) {
        *d++ = ch;
    }

    return dest;
}

int strcmp(const char *str1, const char *str2) {
    for(; *str1 == *str2; ++str1, ++str2) {
        if(*str1 == '\0' && *str2 == '\0') {
            return 0;
        }
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}

int strncmp(const char *str1, const char *str2, uint64_t n) {
    for(uint64_t i = 0; *str1 == *str2 && ++i < n; ++str1, ++str2) {
        if(*str1 == '\0' && *str2 == '\0') {
            return 0;
        }
    }
    return *(unsigned char *)++str1 - *(unsigned char *)++str2;
}

uint64_t strlen(const char *str) {
    uint64_t result = 0;
    while(str[result++] != '\0');
    return result;
}
