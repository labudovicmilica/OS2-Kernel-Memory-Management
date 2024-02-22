//
// Created by os on 6/24/23.
//

#include "../h/slab.hpp"
#include "../h/BuddyAllocator.hpp"
#include "../h/Cache.hpp"

void kmem_init(void *space, int block_num) {
    BuddyAllocator::buddyInit(space, block_num);
    Cache::initCache();
}

// Allocate cache
kmem_cache_t *kmem_cache_create(const char *name, size_t size, void (*ctor)(void *), void (*dtor)(void *)) {
    return (kmem_cache_t *)(new Cache(name, size, ctor, dtor));
}

// Shrink cache
int kmem_cache_shrink(kmem_cache_t *cachep) {
    return ((Cache*)cachep)->shrink();
}

// Allocate one object from cache
void *kmem_cache_alloc(kmem_cache_t *cachep) {
    return ((Cache*)cachep)->alloc();
}

// Deallocate one object from cache
void kmem_cache_free(kmem_cache_t *cachep, void *objp) {
    ((Cache*)cachep)->free(objp);
}

// Alloacate one small memory buffer
void *kmalloc(size_t size) {
    return Cache::kalloc(size);
}

// Deallocate one small memory buffer
void kfree(const void *objp){
    Cache::kfree((void *)(objp));
}

// Deallocate cache
void kmem_cache_destroy(kmem_cache_t *cachep) {
    delete ((Cache*)cachep);
}

// Print cache info
void kmem_cache_info(kmem_cache_t *cachep){
    ((Cache*)cachep)->printInfo();
}

// Print error message
int kmem_cache_error(kmem_cache_t *cachep){
    return ((Cache*)cachep)->printError();
}