#ifndef TYPES_H
#define TYPES_H

// Basic integer types for Yazan Game OS (32-bit x86 bare-metal)
typedef unsigned char      uint8_t;
typedef signed char        int8_t;
typedef unsigned short     uint16_t;
typedef signed short       int16_t;
typedef unsigned int       uint32_t;
typedef signed int         int32_t;
typedef unsigned long long uint64_t;
typedef signed long long   int64_t;

typedef uint32_t           size_t;
typedef int32_t            ssize_t;
typedef uint32_t           uintptr_t;
typedef int32_t            intptr_t;

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef bool
#define bool bool
#endif

// Utility inline functions
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outw(uint16_t port, uint16_t val) {
    __asm__ volatile("outw %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outl(uint16_t port, uint32_t val) {
    __asm__ volatile("outl %0, %1" : : "a"(val), "Nd"(port));
}

static inline void io_wait(void) {
    outb(0x80, 0);
}

static inline void cli(void) {
    __asm__ volatile("cli");
}

static inline void sti(void) {
    __asm__ volatile("sti");
}

static inline void hlt(void) {
    __asm__ volatile("hlt");
}

// Memory operations
void* kmemset(void* dest, int val, size_t count);
void* kmemcpy(void* dest, const void* src, size_t count);
int   kmemcmp(const void* s1, const void* s2, size_t n);
size_t kstrlen(const char* str);
char*  kstrcpy(char* dest, const char* src);
char*  kstrncpy(char* dest, const char* src, size_t n);
int    kstrcmp(const char* s1, const char* s2);
int    kstrncmp(const char* s1, const char* s2, size_t n);
char*  kstrcat(char* dest, const char* src);

#endif // TYPES_H
