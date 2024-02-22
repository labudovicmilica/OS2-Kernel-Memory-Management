//
// Created by os on 6/23/23.
//
#include "../lib/hw.h"
#ifndef PROJECT_BASE_V1_1_BUDDYALLOCATOR_HPP
#define PROJECT_BASE_V1_1_BUDDYALLOCATOR_HPP


class BuddyAllocator {
public:
    struct FreeBuddyList{
        void* addr;
        FreeBuddyList* next;
    };

    BuddyAllocator(const BuddyAllocator&) = delete;
    BuddyAllocator(BuddyAllocator&&) = delete;
    BuddyAllocator operator=(const BuddyAllocator&) = delete;
    BuddyAllocator operator=(BuddyAllocator&&) = delete;

    static const size_t maxDegree = 12;
private:
    BuddyAllocator() = default;
    ~BuddyAllocator();
    //dodaje memorijski blok velicine 2**size blokova u listu sa blokovima te velicine
    static FreeBuddyList* addToList(void* addr, size_t size);
    //pokusava da spoji blok node sa njegovim buddy-jem
    static void tryJoiningBuddies(FreeBuddyList* node, size_t size);
    //racuna logaritam za osnovu 2 broja number i zaokruzuje na prvi veci broj
    static uint64 ceilLog2(uint64 number);
    //racuna broj bloka od node
    static uint64 blockNumber(void* node);
    //vraca 0 ako je broj paran, 1 ako je neparan
    static uint64 mod2(uint64 number);

    //niz listi za smestanje slobodnih memorijski blokova velicine od 1 do 2**(maxDegree-1) blokova od 4096 B
    static FreeBuddyList* buddy[maxDegree];

    static void* KERNEL_HEAP_START_ADDR;
    static void* KERNEL_HEAP_END_ADDR;

public:
    //metoda za inicijalizaciju Buddy alokatora
    static void buddyInit(void* start, int block_num);
    //metoda koja dohvata instancu staticke klase BuddyAllocator
    static BuddyAllocator* getInstance();

    //metoda koja alocira 2**ceilLog2(size) blokova ako je moguce, a ako nije vraca nullptr
    static void* buddy_alloc(size_t size);
    //metoda koja oslobadja memoriju pocevsi od adrese addr i vraca je u listu blokova velicine 2**size
    static int buddy_free(void* addr, size_t size);

    //metoda za ispis buddy listi
    static void printBuddyLists();

};


#endif //PROJECT_BASE_V1_1_BUDDYALLOCATOR_HPP
