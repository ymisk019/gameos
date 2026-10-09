#include "../include/types.h"

void* kmemset(void* dest, int val, size_t count) {
    uint8_t* temp = (uint8_t*)dest;
    for (size_t i = 0; i < count; i++) {
        temp[i] = (uint8_t)val;
    }
    return dest;
}

void* kmemcpy(void* dest, const void* src, size_t count) {
    const uint8_t* sp = (const uint8_t*)src;
    uint8_t* dp = (uint8_t*)dest;
    for (size_t i = 0; i < count; i++) {
        dp[i] = sp[i];
    }
    return dest;
}

int kmemcmp(const void* s1, const void* s2, size_t n) {
    const uint8_t* p1 = (const uint8_t*)s1;
    const uint8_t* p2 = (const uint8_t*)s2;
    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] - p2[i];
        }
    }
    return 0;
}

size_t kstrlen(const char* str) {
    size_t len = 0;
    while (str && str[len] != '\0') {
        len++;
    }
    return len;
}

char* kstrcpy(char* dest, const char* src) {
    size_t i = 0;
    while (src && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return dest;
}

char* kstrncpy(char* dest, const char* src, size_t n) {
    size_t i = 0;
    while (i < n && src && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    while (i < n) {
        dest[i] = '\0';
        i++;
    }
    return dest;
}

int kstrcmp(const char* s1, const char* s2) {
    if (!s1 || !s2) return -1;
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int kstrncmp(const char* s1, const char* s2, size_t n) {
    if (!s1 || !s2) return -1;
    for (size_t i = 0; i < n; i++) {
        if (s1[i] != s2[i] || s1[i] == '\0') {
            return (unsigned char)s1[i] - (unsigned char)s2[i];
        }
    }
    return 0;
}

char* kstrcat(char* dest, const char* src) {
    char* r = dest;
    while (*dest) dest++;
    while (*src) *dest++ = *src++;
    *dest = '\0';
    return r;
}
