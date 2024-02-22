//
// Created by os on 6/24/23.
//

#ifndef PROJECT_BASE_V1_1_SLAB_HPP
#define PROJECT_BASE_V1_1_SLAB_HPP

#include "../lib/hw.h"


typedef struct kmem_cache_s kmem_cache_t;

#define BLOCK_SIZE (4096)

void kmem_init(void *space, int block_num);

// Allocate cache
kmem_cache_t *kmem_cache_create(const char *name, size_t size, void (*ctor)(void *), void (*dtor)(void *));

// Shrink cache
int kmem_cache_shrink(kmem_cache_t *cachep);

// Allocate one object from cache
void *kmem_cache_alloc(kmem_cache_t *cachep);

// Deallocate one object from cache
void kmem_cache_free(kmem_cache_t *cachep, void *objp);

// Alloacate one small memory buffer
void *kmalloc(size_t size);

// Deallocate one small memory buffer
void kfree(const void *objp);

// Deallocate cache
void kmem_cache_destroy(kmem_cache_t *cachep);

// Print cache info
void kmem_cache_info(kmem_cache_t *cachep);

// Print error message
int kmem_cache_error(kmem_cache_t *cachep);


#endif //PROJECT_BASE_V1_1_SLAB_HPP
