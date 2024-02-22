//
// Created by os on 7/25/22.
//

#include "../h/MemoryAllocator.hpp"

MemoryAllocator::FreeMem* MemoryAllocator::freeMemHead = (FreeMem*)HEAP_START_ADDR;

void *MemoryAllocator::mem_alloc(size_t size) {
    getInstance();
    uint64 requiredSize = size * MEM_BLOCK_SIZE;
    FreeMem* current = freeMemHead, *previous = nullptr;
    for (;current != nullptr && requiredSize > current->memSize; previous = current, current = current->next);

    if (current == nullptr) return nullptr;
    if ((char*)current + sizeof(FreeMem) + requiredSize >= HEAP_END_ADDR) return nullptr; //ne bi trebalo da se desi uopste

    uint64 remainingSize = current->memSize - requiredSize;
    if (remainingSize < (sizeof(FreeMem) + MEM_BLOCK_SIZE)) {
        if (previous) previous->next = current->next;
        else freeMemHead = current->next;
    }
    else {
        FreeMem* newCurrent = (FreeMem*)((char*)current + sizeof(FreeMem) + requiredSize);
        if (previous) previous->next = newCurrent;
        else freeMemHead = newCurrent;
        newCurrent->next = current->next;
        newCurrent->memSize = remainingSize - sizeof(FreeMem);
        current->memSize = requiredSize;
    }
    current->next = nullptr;
    //printInt((uint64)((char*)current + sizeof(FreeMem)));
    return (char*)current + sizeof(FreeMem);
}

int MemoryAllocator::mem_free(void *pointer) {
    getInstance();
    if (pointer == nullptr) return -1;
    if (pointer < HEAP_START_ADDR || HEAP_END_ADDR < pointer) return -2;

    uint64 toFreeAddress = (uint64) ((char*)pointer - sizeof(FreeMem));
    FreeMem* current = nullptr;
    if (freeMemHead && toFreeAddress > (uint64)freeMemHead) {
        for(current = freeMemHead; current->next != nullptr && toFreeAddress > (uint64)current->next; current = current->next);
    }

    FreeMem* newSegment = (FreeMem*)toFreeAddress;
    if(current) {
        newSegment->next = current->next;
        current->next = newSegment;
    }
    else {
        newSegment->next = freeMemHead;
        freeMemHead = newSegment;
    }

    //try to join current and newSegment
    if (current != nullptr && (char*)current + sizeof(FreeMem) + current->memSize == (char*)newSegment) {
        current->memSize += +sizeof(FreeMem) + newSegment->memSize;
        current->next = newSegment->next;
        newSegment->next = nullptr;
        newSegment = current;
    }
    FreeMem* nextSegment = newSegment->next;
    if (!nextSegment) return 0;

    //try to join newSegment and segment that follows
    if ((char*)newSegment + sizeof(FreeMem) + newSegment->memSize == (char*)nextSegment) {
        newSegment->memSize += +sizeof(FreeMem) + nextSegment->memSize;
        newSegment->next = nextSegment->next;
        nextSegment->next = nullptr;
    }
    return 0;
}

MemoryAllocator::~MemoryAllocator() {
    FreeMem* old = nullptr;
    while (freeMemHead) {
        old = freeMemHead;
        freeMemHead = freeMemHead->next;
        old->next = nullptr;
    }


}
