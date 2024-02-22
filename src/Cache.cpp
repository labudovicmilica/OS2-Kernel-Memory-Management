//
// Created by os on 7/19/23.
//

#include "../h/Cache.hpp"
#include "../h/slab.hpp"
#include "../h/SlabAllocator.hpp"
#include "../h/print.hpp"

const char* MEM_ERROR = "ERROR: NO MORE MEMORY";

Cache* Cache::headCache = nullptr;
Cache* Cache::allCaches = nullptr;
uint64 Cache::cachesCnt = 0;
Cache* Cache::bufferCachesPointer[MAX_BUFFER_SIZE - MIN_BUFFER_SIZE + 1] = {nullptr};


Cache::Cache(const char *name, size_t size, void (*ctor)(void *), void (*dtor)(void *)) {
    objectSize = size;
    this->ctor = ctor;
    this->dtor = dtor;

    uint64 i = 0;
    while(name[i] != '\0'){
        this->name[i] = name[i];
        i++;
    }

    if (!headCache) {
        headCache = this;
        this->nextCache = nullptr;
    } else {
        this->nextCache = headCache;
        headCache = this;
    }

}

void *Cache::operator new(size_t size) {
    return &allCaches[cachesCnt++];
}

void Cache::operator delete(void *ptr) {
    Cache* prev = nullptr, *cur = headCache;
    for(; cur != nullptr; prev = cur, cur = cur->nextCache) {
        if (cur == (Cache*)ptr) {

            if (prev){
                prev->nextCache = cur->nextCache;
            } else {
                headCache = cur->nextCache;
            }
            cur->nextCache = nullptr;
            break;
        }
    }
}

void Cache::initCache() {
    Cache::bufferCachesPointer[MAX_BUFFER_SIZE - MIN_BUFFER_SIZE + 1] = {nullptr};
    allCaches = (Cache *)(BuddyAllocator::buddy_alloc(1));
}

int Cache::shrink() {
    int numOfBlocks = 0;
    if (!allocatedSlabAfterShrink) {
        for(SlabAllocator *cur = emptySlabs; cur;) {
            SlabAllocator *temp = cur;
            cur = cur->next;
            uint64 cnt = SlabAllocator::deallocateSlab(temp);
            numOfBlocks += cnt;
        }

    }
    allocatedSlabAfterShrink = false;
    return numOfBlocks;
}

void *Cache::alloc() {
    if (partialSlabs != nullptr) {
        SlabAllocator* chosen = partialSlabs;
        void *addr = chosen->getFreeSlot();
        if (chosen->numOfFreeSlots == 0) {
            moveFromPartialToFull(chosen);
        }
        return addr;
    } else if (emptySlabs != nullptr) {
        SlabAllocator* chosen = emptySlabs;
        void *addr = chosen->getFreeSlot();
        if (chosen->numOfFreeSlots == 0) {
            moveFromEmptyToFull(chosen);
        } else {
            moveFromEmptyToPartial(chosen);
        }
        return addr;
    } else {
        SlabAllocator *chosen = nullptr;
        if (!bufferCache) {
            chosen = SlabAllocator::allocateNewSlab(this, objectSize, ctor, dtor);
        } else{
            chosen = SlabAllocator::allocateBufferSlab(this, objectSize);
        }
        if (chosen == nullptr) {
            //nema memorije
            error = MEM_ERROR;
            //printf(error);
            return nullptr;
        }

        allocatedSlabAfterShrink = true;

        void *addr = chosen->getFreeSlot();
        if (chosen->numOfFreeSlots == 0) {
            chosen->next = fullSlabs;
            fullSlabs = chosen;
        } else {
            chosen->next = nullptr;
            partialSlabs = chosen;
        }
        return addr;
    }

}

void Cache::moveFromPartialToFull(SlabAllocator *slab) {
    //uvek se uzima prvi iz partial, pa se uvek on premesta

    if (slab->next){
        partialSlabs = partialSlabs->next;
    } else {
        partialSlabs = nullptr;
    }

    slab->next = fullSlabs;
    fullSlabs = slab;
}

void Cache::moveFromEmptyToPartial(SlabAllocator *slab) {
    if (slab->next) {
        emptySlabs = emptySlabs->next;
    } else {
        emptySlabs = nullptr;
    }

    slab->next = partialSlabs;
    partialSlabs = slab;
}

void Cache::moveFromEmptyToFull(SlabAllocator *slab) {
    if (slab->next) {
        emptySlabs = emptySlabs->next;
    } else {
        emptySlabs = nullptr;
    }

    slab->next = fullSlabs;
    fullSlabs = slab;
}

int Cache::free(void *addr) {
    SlabAllocator* cur = partialSlabs;
    for(; cur != nullptr; cur = cur->next) {
        if ((char*)addr > (char*)cur && (char*)addr < ((char*)(cur) + cur->totalSizeOfSlab)) break;
    }
    if(cur) {
        cur->returnFreeSlot(addr);
        if (cur->totalNumOfFreeSlots == cur->numOfFreeSlots) {
            moveFromPartialToEmpty(cur);
        }
        return 0;
    }

    cur = fullSlabs;
    for(; cur != nullptr; cur = cur->next) {
        if ((char*)addr > (char*)cur && (char*)addr < ((char*)(cur) + cur->totalSizeOfSlab)) break;
    }
    if(cur) {
        cur->returnFreeSlot(addr);
        if (cur->totalNumOfFreeSlots == cur->numOfFreeSlots) {
            moveFromFullToEmpty(cur);
        } else {
            moveFromFullToPartial(cur);
        }
        return 0;
    }
    //inace greska

    return 1;
}

void Cache::moveFromPartialToEmpty(SlabAllocator *slab) {
    SlabAllocator* cur = partialSlabs, *prev = nullptr;
    for(; cur != nullptr; prev = cur, cur = cur->next) {
        if (cur == slab) {
            if (prev) prev->next = slab->next;
            else partialSlabs = partialSlabs->next;
            break;
        }
    }
    slab->next = emptySlabs;
    emptySlabs = slab;
}

void Cache::moveFromFullToEmpty(SlabAllocator *slab) {
    SlabAllocator* cur = fullSlabs, *prev = nullptr;
    for(; cur != nullptr; prev = cur, cur = cur->next) {
        if (cur == slab) {
            if (prev) prev->next = slab->next;
            else fullSlabs = fullSlabs->next;
            break;
        }
    }
    slab->next = emptySlabs;
    emptySlabs = slab;
}

void Cache::moveFromFullToPartial(SlabAllocator *slab) {
    SlabAllocator* cur = fullSlabs, *prev = nullptr;
    for(; cur != nullptr; prev = cur, cur = cur->next) {
        if (cur == slab) {
            if (prev) prev->next = slab->next;
            else fullSlabs = fullSlabs->next;
            break;
        }
    }
    slab->next = partialSlabs;
    partialSlabs = slab;
}

Cache::~Cache() {
    SlabAllocator* temp = nullptr;
    for(SlabAllocator* cur = emptySlabs; cur; cur = temp){
        temp = cur->next;
        SlabAllocator::deallocateSlab(cur);
    }
    for(SlabAllocator* cur = partialSlabs; cur; cur = temp){
        temp = cur->next;
        SlabAllocator::deallocateSlab(cur);
    }
    for(SlabAllocator* cur = fullSlabs; cur; cur = temp){
        temp = cur->next;
        SlabAllocator::deallocateSlab(cur);
    }
}


void *Cache::kalloc(size_t size) {
    uint64 deg = 5;

    while((1<<deg) < (int)size ) {
        deg++;
    }

    uint64 i = deg - MIN_BUFFER_SIZE;

    if (bufferCachesPointer[i] == nullptr) {
        char name[] = "SIZE-00\n";
        name[5] = '0' + i / 10;
        name[6] = '0' + i % 10;

        bufferCachesPointer[i] = new Cache(name, (1 << deg), nullptr, nullptr);
        bufferCachesPointer[i]->bufferCache = true;
    }

    return bufferCachesPointer[i]->alloc();
}

void Cache::kfree(void *addr) {
    for(uint64 i = MIN_BUFFER_SIZE; i <= MAX_BUFFER_SIZE; i++) {
        if(Cache::bufferCachesPointer[i - MIN_BUFFER_SIZE]){
            if (bufferCachesPointer[i - MIN_BUFFER_SIZE]->free(addr) == 0) break;
        }
    }

}

int Cache::printError() {
    if (error) {
        printf(error);
        return 1;
    } else {
        return 0;
    }
}

void Cache::printInfo() {
    printf("\n\n");
    printf("CACHE: "); printf(name); printf("\n\n");
    printf("Object size: %dB\n", objectSize);

    int totalNumberOfSlots = 0, totalSpace = 0;

    if(partialSlabs) {
        totalNumberOfSlots = partialSlabs->totalNumOfFreeSlots;
        totalSpace = partialSlabs->totalSizeOfSlab;
    } else if(fullSlabs) {
        totalNumberOfSlots = fullSlabs->totalNumOfFreeSlots;
        totalSpace = partialSlabs->totalSizeOfSlab;

    } else {
        totalNumberOfSlots = emptySlabs->totalNumOfFreeSlots;
        totalSpace = partialSlabs->totalSizeOfSlab;

    }
    printf("Number of objects in one slab: %d\n", totalNumberOfSlots);


    int cacheSizeInBlocks = 0;
    int numberOfSlabs = 0;
    int numberOfFreeSlots = 0;

    printInfoHelper(&numberOfSlabs, &numberOfFreeSlots);

    cacheSizeInBlocks = numberOfSlabs*totalSpace/BLOCK_SIZE;
    printf("Cache size in blocks: %d\n\n", cacheSizeInBlocks);

    printf("Number of slabs in cache: %d\n", numberOfSlabs);

    printf("Percent of allocated slots in cache: %d\n\n", (totalNumberOfSlots - numberOfFreeSlots)* 100 / totalNumberOfSlots);

    printf("-------------------------------------\n\n");

}

void Cache::printInfoHelper(int* slabs, int* slots) {
    SlabAllocator* cur;

    for (cur = partialSlabs; cur; cur = cur->next) {
        (*slabs)++;
        *slots += cur->numOfFreeSlots;

    }
    for (cur = fullSlabs; cur; cur = cur->next) {
        (*slabs)++;
        *slots += cur->numOfFreeSlots;

    }
    for (cur = emptySlabs; cur; cur = cur->next) {
        (*slabs)++;
        *slots += cur->numOfFreeSlots;

    }

}
