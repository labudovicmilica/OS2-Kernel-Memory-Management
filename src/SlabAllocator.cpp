//
// Created by os on 7/21/23.
//

#include "../h/SlabAllocator.hpp"
#include "../h/slab.hpp"

SlabAllocator *SlabAllocator::allocateNewSlab(Cache *cache, size_t size, void (*ctor)(void *), void (*dtor)(void *)) {
    uint64 slot = sizeof(FreeSlot) + size;
    uint64 reqSize = slot + sizeof(SlabAllocator);
    uint64 number = (reqSize - 1)/BLOCK_SIZE + 1;

    SlabAllocator* slab = (SlabAllocator*) BuddyAllocator::buddy_alloc(number);

    if (slab == nullptr) return nullptr;

    slab->objectSize = size;
    slab->slotSize = slot;
    slab->ctor = ctor;
    slab->dtor = dtor;
    slab->ownerCache = cache;
    slab->bufferSlab = false;

    number--;
    uint64 cnt = 0;
    while (number > 0) {
        number >>= 1;
        cnt++;
    }
    slab->totalSizeOfSlab = (1<<cnt)*BLOCK_SIZE;
    slab->totalNumOfFreeSlots = ((1<<cnt)*BLOCK_SIZE - sizeof(SlabAllocator)) / slot;
    slab->numOfFreeSlots = slab->totalNumOfFreeSlots;
    slab->freeSlotHead = (FreeSlot*)((char *) slab + sizeof(SlabAllocator));
    FreeSlot* cur = slab->freeSlotHead, *curNext = nullptr;

    for(uint64 i = 0; i < (slab->totalNumOfFreeSlots - 1); i++) {
        curNext = (FreeSlot*)((char*)cur + slot);
        cur->next = curNext;

        if(ctor) {
            ctor((char*)cur + sizeof(FreeSlot));
        }
        cur = curNext;

    }
    cur->next = nullptr;

    return slab;
}

void *SlabAllocator::getFreeSlot() {
    if (numOfFreeSlots == 0) return nullptr;
    else {
        numOfFreeSlots--;
        void* addr = (char*)freeSlotHead + sizeof(FreeSlot);
        freeSlotHead = freeSlotHead->next;
        return addr;
    }

}

void SlabAllocator::returnFreeSlot(void* addr) {
    FreeSlot* slot = (FreeSlot*)((char*)addr - sizeof(FreeSlot));
    slot->next = freeSlotHead;
    freeSlotHead = slot;
    numOfFreeSlots++;

    if(!bufferSlab) {
        if (dtor) {
            dtor(addr);
        }
/*
        if (ctor) {
            ctor(addr);
        }*/
    }
}

uint64 SlabAllocator::deallocateSlab(SlabAllocator *slab) {
    return BuddyAllocator::buddy_free(slab, (slab->slotSize + sizeof(SlabAllocator)-1)/BLOCK_SIZE + 1);
}

SlabAllocator *SlabAllocator::allocateBufferSlab(Cache *cache, size_t size) {
    uint64 slot = sizeof(FreeSlot) + size;
    uint64 reqSize = slot + sizeof(SlabAllocator);
    uint64 number = (reqSize - 1)/BLOCK_SIZE + 1;

    SlabAllocator* slab = (SlabAllocator*) BuddyAllocator::buddy_alloc(number);

    if (slab == nullptr) return nullptr;

    slab->objectSize = size;
    slab->slotSize = slot;
    slab->ownerCache = cache;
    slab->bufferSlab = true;

    number--;
    uint64 cnt = 0;
    while (number > 0) {
        number >>= 1;
        cnt++;
    }
    slab->totalSizeOfSlab = (1<<cnt)*BLOCK_SIZE;
    slab->totalNumOfFreeSlots = ((1<<cnt)*BLOCK_SIZE - sizeof(SlabAllocator)) / slot;
    slab->numOfFreeSlots = slab->totalNumOfFreeSlots;
    slab->freeSlotHead = (FreeSlot*)((char *) slab + sizeof(SlabAllocator));
    FreeSlot* cur = slab->freeSlotHead, *curNext = nullptr;

    for(uint64 i = 0; i < (slab->totalNumOfFreeSlots - 1); i++) {
        curNext = (FreeSlot*)((char*)cur + slot);
        cur->next = curNext;

        cur = curNext;

    }
    cur->next = nullptr;

    return slab;
}

uint64 SlabAllocator::deallocateBufferSlab(SlabAllocator *slab) {
    return BuddyAllocator::buddy_free(slab, (slab->slotSize + sizeof(SlabAllocator)-1)/BLOCK_SIZE + 1);
}



