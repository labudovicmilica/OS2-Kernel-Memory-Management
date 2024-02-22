//
// Created by os on 7/27/22.
//

#ifndef PROJECT_BASE_V1_1_SYSCALL_C_HPP
#define PROJECT_BASE_V1_1_SYSCALL_C_HPP

#include "../lib/hw.h"
#include "Cache.hpp"

void* mem_alloc(size_t size);

int mem_free(void* p);

class TCB;
typedef TCB _thread;
typedef _thread* thread_t;

int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg);
int thread_create_only(thread_t* handle, void (*start_routine)(void*), void* arg);

void thread_dispatch();

int thread_exit();

int thread_start(thread_t handle);

class Sem;
typedef Sem _sem;
typedef _sem* sem_t;

int sem_open(sem_t* handle, unsigned int init);

int sem_close(sem_t id);

int sem_wait(sem_t id);

int sem_signal(sem_t id);

typedef unsigned long time_t;
int time_sleep(time_t time);

const int EOF = -1;

char getc();

void putc(char c);

#endif //PROJECT_BASE_V1_1_SYSCALL_C_HPP
