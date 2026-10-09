#include "../include/memory.h"

// Simple, fast buddy/bump heap allocator for Yazan Game OS
static uint32_t heap_start_addr = 0;
static uint32_t heap_end_addr = 0;
static uint32_t heap_curr_addr = 0;

struct BlockHeader {
    uint32_t size;
    bool is_free;
    BlockHeader* next;
};

static BlockHeader* free_list_head = NULL;

void memory_init(uint32_t mem_start, uint32_t mem_size) {
    // Align start to 16 bytes
    heap_start_addr = (mem_start + 15) & ~15;
    heap_end_addr = mem_start + mem_size;
    heap_curr_addr = heap_start_addr;

    free_list_head = (BlockHeader*)heap_start_addr;
    free_list_head->size = mem_size - sizeof(BlockHeader);
    free_list_head->is_free = true;
    free_list_head->next = NULL;
}

void* kmalloc(size_t size) {
    if (size == 0) return NULL;
    
    // 16-byte alignment
    size = (size + 15) & ~15;

    BlockHeader* current = free_list_head;
    while (current) {
        if (current->is_free && current->size >= size) {
            // Can split block?
            if (current->size >= size + sizeof(BlockHeader) + 32) {
                BlockHeader* next_block = (BlockHeader*)((uint8_t*)current + sizeof(BlockHeader) + size);
                next_block->size = current->size - size - sizeof(BlockHeader);
                next_block->is_free = true;
                next_block->next = current->next;

                current->size = size;
                current->next = next_block;
            }
            current->is_free = false;
            return (void*)((uint8_t*)current + sizeof(BlockHeader));
        }
        current = current->next;
    }

    return NULL; // Out of memory
}

void* kmalloc_aligned(size_t size, size_t alignment) {
    // Basic aligned allocation
    void* ptr = kmalloc(size + alignment + sizeof(void*));
    if (!ptr) return NULL;

    uintptr_t raw = (uintptr_t)ptr + sizeof(void*);
    uintptr_t aligned = (raw + (alignment - 1)) & ~(alignment - 1);
    ((void**)aligned)[-1] = ptr;
    return (void*)aligned;
}

void kfree(void* ptr) {
    if (!ptr) return;

    BlockHeader* header = (BlockHeader*)((uint8_t*)ptr - sizeof(BlockHeader));
    header->is_free = true;

    // Coalesce adjacent free blocks
    BlockHeader* current = free_list_head;
    while (current && current->next) {
        if (current->is_free && current->next->is_free) {
            current->size += sizeof(BlockHeader) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

void* krealloc(void* ptr, size_t new_size) {
    if (!ptr) return kmalloc(new_size);
    if (new_size == 0) {
        kfree(ptr);
        return NULL;
    }

    BlockHeader* header = (BlockHeader*)((uint8_t*)ptr - sizeof(BlockHeader));
    if (header->size >= new_size) {
        return ptr;
    }

    void* new_ptr = kmalloc(new_size);
    if (new_ptr) {
        kmemcpy(new_ptr, ptr, header->size);
        kfree(ptr);
    }
    return new_ptr;
}

uint32_t memory_get_total(void) {
    return heap_end_addr - heap_start_addr;
}

uint32_t memory_get_used(void) {
    uint32_t used = 0;
    BlockHeader* cur = free_list_head;
    while (cur) {
        if (!cur->is_free) {
            used += cur->size + sizeof(BlockHeader);
        }
        cur = cur->next;
    }
    return used;
}

uint32_t memory_get_free(void) {
    uint32_t free_mem = 0;
    BlockHeader* cur = free_list_head;
    while (cur) {
        if (cur->is_free) {
            free_mem += cur->size;
        }
        cur = cur->next;
    }
    return free_mem;
}

// C++ operator new/delete definitions
void* operator new(size_t size) {
    return kmalloc(size);
}

void* operator new[](size_t size) {
    return kmalloc(size);
}

void operator delete(void* p) noexcept {
    kfree(p);
}

void operator delete[](void* p) noexcept {
    kfree(p);
}

void operator delete(void* p, size_t) noexcept {
    kfree(p);
}

void operator delete[](void* p, size_t) noexcept {
    kfree(p);
}
