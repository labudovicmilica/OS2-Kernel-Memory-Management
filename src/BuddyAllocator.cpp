//
// Created by os on 6/23/23.
//

#include "../h/BuddyAllocator.hpp"
#include "../h/slab.hpp"
#include "../h/print.hpp"

void* BuddyAllocator::KERNEL_HEAP_START_ADDR = nullptr;
void* BuddyAllocator::KERNEL_HEAP_END_ADDR = nullptr;
BuddyAllocator::FreeBuddyList* BuddyAllocator::buddy[] = {};

BuddyAllocator *BuddyAllocator::getInstance() {
    static BuddyAllocator* buddyAllocator;
    return buddyAllocator;
}

void BuddyAllocator::buddyInit(void* start, int block_num) {
    KERNEL_HEAP_START_ADDR = start;
    KERNEL_HEAP_END_ADDR = (void*)((char*)start + (1UL << 24));
    HEAP_START_ADDR = (void*)((char*)start + (1UL << 24));

    for (uint64 i = 0; i < (maxDegree - 1); i++) {
        buddy[i] = nullptr;
    }
    FreeBuddyList* max = (FreeBuddyList*) KERNEL_HEAP_START_ADDR;
    max->addr = KERNEL_HEAP_START_ADDR;
    max->next = nullptr;

    buddy[maxDegree - 1] = max;

}

void *BuddyAllocator::buddy_alloc(size_t reqSize) {

    uint64 size = ceilLog2(reqSize);

    if (size >= maxDegree) return nullptr;

    if (buddy[size] != nullptr) {
        FreeBuddyList* ret = buddy[size];
        buddy[size] = buddy[size]->next;
        return ret;
    }


    for (uint64 cur = size + 1; cur < maxDegree; cur++) {
        if (buddy[cur] != nullptr) {
            FreeBuddyList* node = buddy[cur];
            buddy[cur] = buddy[cur]->next;
            uint64 upper = cur;
            while (upper > 0 && --upper >= size) {
                addToList((char*)node->addr + (1UL << upper) * BLOCK_SIZE, upper);

            }
            return (void*) node;
        }
    }

    return nullptr;
}

int BuddyAllocator::buddy_free(void *addr, size_t reqSize) {

    uint64 size = ceilLog2(reqSize);

    if (size < 0 || size >= maxDegree) return -1;
    if (addr < KERNEL_HEAP_START_ADDR || addr >= KERNEL_HEAP_END_ADDR) return -2;

    FreeBuddyList* node = addToList(addr, size);
    tryJoiningBuddies(node, size);

    //vraca broj blokova koji se oslobadja
    return (1 << size);
}

BuddyAllocator::~BuddyAllocator() {
    for (uint64 i = 0; i < (maxDegree - 1); i++) {
        buddy[i] = nullptr;
    }
    FreeBuddyList* max = (FreeBuddyList*) KERNEL_HEAP_START_ADDR;
    max->addr = KERNEL_HEAP_START_ADDR;
    max->next = nullptr;

    buddy[maxDegree - 1] = max;
}

BuddyAllocator::FreeBuddyList* BuddyAllocator::addToList(void *addr, size_t size) {
    FreeBuddyList* node = (FreeBuddyList*)addr;
    node->addr = addr;
    node->next = nullptr;
    if (buddy[size] == nullptr) {
        buddy[size] = node;
        return node;
    }
    FreeBuddyList* prev = nullptr;
    FreeBuddyList* cur = buddy[size];
    for (; cur != nullptr && cur->addr < node->addr; prev = cur, cur = cur->next);

    if (prev == nullptr){
        node->next = buddy[size];
        buddy[size] = node;
    } else {
        node->next = cur;
        prev->next = node;
    }
    return node;
}

void BuddyAllocator::tryJoiningBuddies(BuddyAllocator::FreeBuddyList *node, size_t size) {
    FreeBuddyList* beforePrev = nullptr;
    FreeBuddyList* prev = nullptr;
    FreeBuddyList* cur = buddy[size];
    for (; cur != nullptr && cur->addr != node->addr; beforePrev = prev, prev = cur, cur = cur->next);
    FreeBuddyList* following = cur->next;

    if (blockNumber((char*)cur->addr + (1UL << size)*BLOCK_SIZE)/2 != blockNumber((char*)cur->addr) && following != nullptr) {
        if (((char*)cur->addr + (1UL<<size)*BLOCK_SIZE) == (char*)following->addr) {

            FreeBuddyList* newBuddy = addToList(cur->addr, size + 1);
            if (prev != nullptr){
                prev->next = following->next;
            } else {
                buddy[size] = following->next;
            }
            tryJoiningBuddies(newBuddy, size + 1);
        }
    } else if (blockNumber((char*)cur->addr + (1UL << size)*BLOCK_SIZE)/2 == blockNumber((char*)cur->addr) && prev != nullptr) {
        if (((char*)prev->addr + (1UL<<size)*BLOCK_SIZE) == (char*)cur->addr) {
            FreeBuddyList* newBuddy = addToList(prev->addr, size + 1);
            if (beforePrev != nullptr) {
                beforePrev->next = cur->next;
            } else {
                buddy[size] = cur->next;
            }
            tryJoiningBuddies(newBuddy, size + 1);
        }
    }


}

uint64 BuddyAllocator::ceilLog2(uint64 number) {
    number--;
    uint64 cnt = 0;
    while (number > 0) {
        number >>= 1;
        cnt++;
    }
    return cnt;
}

uint64 BuddyAllocator::blockNumber(void *node) {
    return ((char*)node - (char*)KERNEL_HEAP_START_ADDR)/BLOCK_SIZE;
}

uint64 BuddyAllocator::mod2(uint64 number) {
    uint64 res = number / 2;
    return number - res * 2;
}

void BuddyAllocator::printBuddyLists() {
    printf("Prikaz strukture Buddy alokatora:\n\n");
    uint64 i = 0;
    while(i < maxDegree) {
        if (buddy[i] != nullptr) {
            uint64 brBloka = blockNumber(buddy[i]->addr);
            printf("Lista blokova velicine 2^");
            printInt(i);
            printf(" :  ");
            printInt(brBloka);
            FreeBuddyList* node = buddy[i];
            while (node->next) {
                node = node->next;
                brBloka = blockNumber(node->addr);
                printf(" -> ");
                printInt(brBloka);

            }
            printf("\n");
        }
        i++;
    }
    printf("\n\n");
}
