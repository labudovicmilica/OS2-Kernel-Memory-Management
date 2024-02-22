//
// Created by os on 7/27/22.
//

#include "../h/syscall_c.hpp"
#include "../h/TCB.hpp"


void* mem_alloc(size_t size) {

    uint64 sizeInBlocks = size / MEM_BLOCK_SIZE;
    if (sizeInBlocks * MEM_BLOCK_SIZE < size ) sizeInBlocks += 1;
    __asm__ volatile("mv a1, %0" : : "r" (sizeInBlocks));
    __asm__ volatile("li a0, 0x01");
    __asm__ volatile("ecall");

    void* pointer;
    __asm__ volatile("mv %0, a0" : "=r" (pointer));
    return pointer;
}

int mem_free(void* p) {

    __asm__ volatile("mv a1, %0" :: "r" (p));
    __asm__ volatile("li a0, 0x02");
    __asm__ volatile("ecall");

    uint64 val;
    __asm__ volatile("mv %0, a0" : "=r" (val));
    return val;
}

int thread_create(thread_t *handle, void (*start_routine)(void *), void *arg) {
    void* stack_space = mem_alloc(2*DEFAULT_STACK_SIZE);
    __asm__ volatile("mv a4, %0" : : "r" (stack_space));
    __asm__ volatile("mv a3, %0" : : "r" (arg));
    __asm__ volatile("mv a2, %0" : : "r" (start_routine));
    __asm__ volatile("mv a1, %0" : : "r" (handle));
    __asm__ volatile("li a0, 0x11");
    __asm__ volatile("ecall");

    uint64 val;
    __asm__ volatile("mv %0, a0" : "=r" (val));
    return val;
}

void thread_dispatch() {
    __asm__ volatile("li a0, 0x13");
    __asm__ volatile("ecall");
}

int thread_exit() {
    __asm__ volatile("li a0, 0x12");
    __asm__ volatile("ecall");

    return 0;
}

int thread_start(thread_t handle) {
    __asm__ volatile("mv a1, %0" : : "r" (handle));
    __asm__ volatile("li a0, 0x14");
    __asm__ volatile("ecall");

    uint64 val;
    __asm__ volatile("mv %0, a0" : "=r" (val));
    return val;

}

int thread_create_only(thread_t *handle, void (*start_routine)(void *), void *arg) {
    void* stack_space = mem_alloc(2*DEFAULT_STACK_SIZE);
    __asm__ volatile("mv a4, %0" : : "r" (stack_space));
    __asm__ volatile("mv a3, %0" : : "r" (arg));
    __asm__ volatile("mv a2, %0" : : "r" (start_routine));
    __asm__ volatile("mv a1, %0" : : "r" (handle));
    __asm__ volatile("li a0, 0x15");
    __asm__ volatile("ecall");

    uint64 val;
    __asm__ volatile("mv %0, a0" : "=r" (val));
    return val;
}

int sem_open(sem_t *handle, unsigned int init) {
    __asm__ volatile("mv a2, %0" : : "r" (init));
    __asm__ volatile("mv a1, %0" : : "r" (handle));
    __asm__ volatile("li a0, 0x21");
    __asm__ volatile("ecall");

    uint64 val;
    __asm__ volatile("mv %0, a0" : "=r" (val));
    return val;
}

int sem_close(sem_t id) {
    if (id == nullptr) return -1;
    __asm__ volatile("mv a1, %0" : : "r" (id));
    __asm__ volatile("li a0, 0x22");
    __asm__ volatile("ecall");

    uint64 val;
    __asm__ volatile("mv %0, a0" : "=r" (val));
    return val;
}

int sem_wait(sem_t id) {
    if (id == nullptr) return -1;
    __asm__ volatile("mv a1, %0" : : "r" (id));
    __asm__ volatile("li a0, 0x23");
    __asm__ volatile("ecall");

    uint64 val;
    __asm__ volatile("mv %0, a0" : "=r" (val));
    return val;
}

int sem_signal(sem_t id) {
    if (id == nullptr) return -1;
    __asm__ volatile("mv a1, %0" : : "r" (id));
    __asm__ volatile("li a0, 0x24");
    __asm__ volatile("ecall");

    uint64 val;
    __asm__ volatile("mv %0, a0" : "=r" (val));
    return val;
}

int time_sleep(time_t time) {
    __asm__ volatile("mv a1, %0" : : "r" (time));
    __asm__ volatile("li a0, 0x31");
    __asm__ volatile("ecall");

    uint64 val;
    __asm__ volatile("mv %0, a0" : "=r" (val));
    return val;
}


char getc() {
    __asm__ volatile("li a0, 0x41");
    __asm__ volatile("ecall");

    uint64 val;
    __asm__ volatile("mv %0, a0" : "=r" (val));
    return val;
}

void putc(char c) {
    __asm__ volatile("mv a1, %0" : : "r" (c));
    __asm__ volatile("li a0, 0x42");
    __asm__ volatile("ecall");

}
