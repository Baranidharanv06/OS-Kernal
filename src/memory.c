#include <stdint.h>
#include <stddef.h>

#define HEAP_START 0x200000
#define HEAP_SIZE  0x100000

static uintptr_t current_heap = HEAP_START;
static uintptr_t heap_end = HEAP_START + HEAP_SIZE;

static inline uintptr_t align(uintptr_t addr, size_t a) {
    return (addr + (a - 1)) & ~(a - 1);
}

void *kmalloc(size_t size) {
    if (size == 0) return (void*)0;
    uintptr_t aligned = align(current_heap, 8);
    if (aligned + size > heap_end) return (void*)0;
    current_heap = aligned + size;
    return (void*)aligned;
}

void kfree(void *ptr) {
    (void)ptr;
}

void reset_heap() {
    current_heap = HEAP_START;
}
