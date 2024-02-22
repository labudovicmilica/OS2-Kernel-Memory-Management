//
// Created by os on 8/1/22.
//

#include "../h/syscall_cpp.hpp"

void* operator new(uint64 n) {
    return mem_alloc(n);
}

void* operator new[](uint64 n) {
    return mem_alloc(n);
}

void operator delete(void* p) noexcept {
    mem_free(p);
}

void operator delete[](void* p) noexcept {
    mem_free(p);
}




Thread::Thread(void (*body)(void *), void *arg) {
    thread_create_only(&myHandle, body, arg);

}

Thread::Thread() {
    thread_create_only(&myHandle, wrapper, this);

}

Thread::~Thread() {
    //thread_exit();
    delete &myHandle;
}

int Thread::start() {
    if (myHandle) {
        return thread_start(myHandle);
    } else {
        return -1;
    }
}

void Thread::dispatch() {
    thread_dispatch();
}

void wrapper(void *arg) {
    ((Thread*)(arg))->run();
}



Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}



char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}

int Thread::sleep(time_t time) {
    return time_sleep(time);
}

void periodicWrapper(void *arg) {
    Params* par = (Params*)arg;
    while(true) {
        ((PeriodicThread*)(par->arg))->periodicActivation();
        Thread::sleep(par->time);
    }
}

PeriodicThread::PeriodicThread(time_t period) : Thread(periodicWrapper, new Params(this, period)){ }


