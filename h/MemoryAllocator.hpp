#ifndef MEMORYALLOCATOR_HPP
#define MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

class MemoryAllocator {
private:
    struct FreeMem {
        FreeMem* next;
        FreeMem* prev;
        size_t size;
    };

    static bool initialized;
    static FreeMem* freeMemHead;

    static void initialize();

    MemoryAllocator() = default;
    ~MemoryAllocator() = default;

public:
    static void* mem_alloc(size_t size);
    static int mem_free(void*);
};

#endif
