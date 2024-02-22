//
// Created by os on 7/21/23.
//

#ifndef PROJECT_BASE_V1_1_SLABALLOCATOR_HPP
#define PROJECT_BASE_V1_1_SLABALLOCATOR_HPP

#include "../h/print.hpp"
#include "Cache.hpp"
#include "BuddyAllocator.hpp"


class SlabAllocator {

public:
    struct FreeSlot {
        FreeSlot *next;
    };

    SlabAllocator* next;

private:

    friend class Cache;

    size_t objectSize;

    FreeSlot* freeSlotHead;
    uint64 slotSize;
    uint64 totalSizeOfSlab;

    Cache* ownerCache;
    Cache::FUNCTION ctor, dtor;

    bool bufferSlab;




public:

    uint64 numOfFreeSlots;
    uint64 totalNumOfFreeSlots;

    static SlabAllocator* allocateNewSlab(Cache *cache, size_t size, void (*ctor)(void*), void (*dtor)(void*));
    static uint64 deallocateSlab(SlabAllocator* slab);

    static SlabAllocator* allocateBufferSlab(Cache* cache, size_t size);
    static uint64 deallocateBufferSlab(SlabAllocator* slab);

    void* getFreeSlot();
    void returnFreeSlot(void* addr);

};


#endif //PROJECT_BASE_V1_1_SLABALLOCATOR_HPP
