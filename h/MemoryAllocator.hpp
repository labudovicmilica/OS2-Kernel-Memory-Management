//
// Created by os on 7/25/22.
//

#ifndef PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP

#include "../lib/hw.h"


class MemoryAllocator{
public:

    static MemoryAllocator* getInstance() {
        static MemoryAllocator* memoryAllocator;
        static bool flag = false;
        if (!flag) {
            freeMemHead = (FreeMem*)HEAP_START_ADDR;
            freeMemHead->memSize = uint64 (HEAP_END_ADDR) - uint64 (HEAP_START_ADDR) - sizeof(FreeMem);
            freeMemHead->next = nullptr;
            flag = true;
        }

        return memoryAllocator;
    }

    struct FreeMem {
        size_t memSize;
        FreeMem* next;
    };

    static void* mem_alloc(size_t size);
    static int mem_free(void* pointer);

    MemoryAllocator(const MemoryAllocator&) = delete;
    MemoryAllocator(MemoryAllocator&&) = delete;
    MemoryAllocator operator=(const MemoryAllocator&) = delete;
    MemoryAllocator operator=(MemoryAllocator&&) = delete;
    ~MemoryAllocator();

private:
    static FreeMem* freeMemHead;

    MemoryAllocator() = default;

};


#endif //PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP
