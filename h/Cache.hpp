//
// Created by os on 7/19/23.
//

#ifndef PROJECT_BASE_V1_1_CACHE_HPP
#define PROJECT_BASE_V1_1_CACHE_HPP


//#include "SlabAllocator.hpp"

#include "../lib/hw.h"

class SlabAllocator;

class Cache {
public:
    using FUNCTION = void (*)(void*);
    static const uint64 MAX_NAME_LENGTH = 14;

    void* operator new(size_t size);
    void operator delete(void* ptr);
    const char* error = nullptr;

private:
    SlabAllocator *fullSlabs = nullptr, *partialSlabs = nullptr, *emptySlabs = nullptr;

    Cache* nextCache;

    friend class SlabAllocator;

    char name[MAX_NAME_LENGTH];
    uint64 objectSize;
    FUNCTION ctor, dtor;

    static Cache* headCache;
    static Cache* allCaches;
    static uint64 cachesCnt; //predstavlja broj ukupno alociranih keseva do sad, a ne aktivnih

    void moveFromPartialToFull(SlabAllocator* slab);
    void moveFromPartialToEmpty(SlabAllocator* slab);
    void moveFromEmptyToPartial(SlabAllocator* slab);
    void moveFromEmptyToFull(SlabAllocator* slab);
    void moveFromFullToEmpty(SlabAllocator* slab);
    void moveFromFullToPartial(SlabAllocator* slab);

    bool allocatedSlabAfterShrink = true;

    //-------- buffer caches -------------------
    static const uint64 MIN_BUFFER_SIZE = 5;
    static const uint64 MAX_BUFFER_SIZE = 17;

    bool bufferCache = false;

    static Cache* bufferCachesPointer[MAX_BUFFER_SIZE - MIN_BUFFER_SIZE + 1];

    void printInfoHelper(int* slabs, int* slots);

public:

    Cache(const char *name, size_t size, void (*ctor)(void*), void (*dtor)(void*));

    ~Cache();

    static void initCache();

    void* alloc();
    int free(void* addr);

    int shrink();

    static void* kalloc(size_t size);
    static void kfree(void* addr);

    int printError();
    void printInfo();
};


#endif //PROJECT_BASE_V1_1_CACHE_HPP
