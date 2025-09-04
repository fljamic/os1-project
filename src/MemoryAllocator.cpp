#include "../h/MemoryAllocator.hpp"

bool MemoryAllocator::initialized = false;
MemoryAllocator::FreeMem* MemoryAllocator::freeMemHead = 0;

void MemoryAllocator::initialize() {
    freeMemHead = (FreeMem*)HEAP_START_ADDR;
    freeMemHead->next = nullptr;
    freeMemHead->prev = nullptr;
    freeMemHead->size = (size_t)HEAP_END_ADDR - (size_t)HEAP_START_ADDR - sizeof(FreeMem);
    initialized = true;
}

void* MemoryAllocator::mem_alloc(size_t size) {
    if (!initialized)
        MemoryAllocator::initialize();

    if (size <= 0) return nullptr;

    size_t aligned = ((size + MEM_BLOCK_SIZE - 1)/MEM_BLOCK_SIZE)*MEM_BLOCK_SIZE;
    FreeMem* cur = freeMemHead;
    for(; cur; cur = cur->next)
        if(cur->size >= aligned) break;

    if (!cur) return nullptr;

    size_t remaining = cur->size - aligned;

    if (remaining > sizeof(FreeMem)) {
        FreeMem* newStart = (FreeMem*)((size_t)cur + sizeof(FreeMem) + aligned);

        newStart->next = cur->next;
        newStart->prev = cur->prev;
        newStart->size = remaining - sizeof(FreeMem);

        if (cur->prev)
            cur->prev->next = newStart;
        else
            freeMemHead = newStart;

        if (cur->next)
            cur->next->prev = newStart;

        cur->size = aligned;
    }
    else {
        if (cur->prev)
            cur->prev->next = cur->next;
        else
            freeMemHead = cur->next;

        if (cur->next)
            cur->next->prev = cur->prev;
    }
    return (void*)((size_t)cur + sizeof(FreeMem));
}

int MemoryAllocator::mem_free(void* adress) {
    if (!initialized)
        MemoryAllocator::initialize();

    FreeMem* toFree = (FreeMem*)((size_t)adress - sizeof(FreeMem));

    if (!freeMemHead) {
        freeMemHead = toFree;
    }
    else {
        if ((size_t)freeMemHead > (size_t)toFree) {
            toFree->next = freeMemHead;
            freeMemHead->prev = toFree;
            freeMemHead = toFree;
        }
        else {
            FreeMem* cur = freeMemHead;

            while (cur->next && (size_t)(cur->next) < (size_t)toFree)
                cur = cur->next;

            toFree->next = cur->next;

            if (cur->next)
                toFree->next->prev = toFree;

            cur->next = toFree;
            toFree->prev = cur;
        }

        if (toFree->next && (size_t)(toFree->next) == (size_t)toFree + sizeof(FreeMem) + toFree->size) {
            toFree->size += (sizeof(FreeMem) + toFree->next->size);
            toFree->next = toFree->next->next;
        }
        if (toFree->prev && (size_t)toFree == (size_t)(toFree->prev) + sizeof(FreeMem) + toFree->prev->size) {
            toFree->prev->size += (sizeof(FreeMem) + toFree->size);
            toFree->prev->next = toFree->next;
        }
    }
    return 0;
}
