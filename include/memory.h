#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

void memory_init(uint32_t mem_start, uint32_t mem_size);
void* kmalloc(size_t size);
void* kmalloc_aligned(size_t size, size_t alignment);
void  kfree(void* ptr);
void* krealloc(void* ptr, size_t new_size);

uint32_t memory_get_total(void);
uint32_t memory_get_used(void);
uint32_t memory_get_free(void);

#ifdef __cplusplus
}

// C++ operator new and delete
void* operator new(size_t size);
void* operator new[](size_t size);
void  operator delete(void* p) noexcept;
void  operator delete[](void* p) noexcept;
void  operator delete(void* p, size_t size) noexcept;
void  operator delete[](void* p, size_t size) noexcept;

inline void* operator new(size_t, void* ptr) { return ptr; }
inline void* operator new[](size_t, void* ptr) { return ptr; }

#endif

#endif // MEMORY_H
