//
// Created by os on 7/25/22.
//


#include "../h/syscall_c.hpp"
#include "../h/print.hpp"
#include "../h/Riscv.hpp"
#include "../h/TCB.hpp"
#include "../h/BuddyAllocator.hpp"
#include "../h/slab.hpp"
#include "../h/Cache.hpp"

//void userMain();
void userMain1();
void userMain2();

bool userFinished = false;


void wrapperMain(void* arg){
    userMain1();
    userFinished = true;
}

void main() {

    Riscv::w_stvec((uint64) Riscv::supervisorTrap);
    kmem_init((void*)HEAP_START_ADDR, 2048);

    bool test1 = true;

    if(test1) {

        void *stack = MemoryAllocator::mem_alloc(2*DEFAULT_STACK_SIZE / 64);
        void *stack0 = MemoryAllocator::mem_alloc(2*DEFAULT_STACK_SIZE / 64);
        TCB *p, *main;
        TCB::create_thread(&main, nullptr, nullptr, stack0);
        TCB::create_thread(&p, wrapperMain, nullptr, stack);

        TCB::running = main;
        TCB::yield();

        while (!userFinished) {
            TCB::yield();
        }
/*
        __asm__ volatile("li a0, 0x0");
        __asm__ volatile("ecall");*/
    } else {
        userMain2();
    }


}