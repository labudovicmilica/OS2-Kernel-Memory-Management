//
// Created by os on 8/1/22.
//

#ifndef PROJECT_BASE_V1_1_SYSCALL_CPP_HPP
#define PROJECT_BASE_V1_1_SYSCALL_CPP_HPP


#include "../h/syscall_c.hpp"

void* operator new(uint64 n);

void* operator new[](uint64 n);

void operator delete(void* p) noexcept;

void operator delete[](void* p) noexcept;

void wrapper(void* arg);

struct Params {
    void* arg;
    time_t time;

    Params(void* arg, time_t time) : arg(arg), time(time) {}
};

class Thread {
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();
    int start ();
    static void dispatch ();
    static int sleep (time_t);

    friend void wrapper(void* arg);
protected:
    Thread ();
    virtual void run () {}
private:
    thread_t myHandle;
};


class Semaphore {
public:
    Semaphore (unsigned int init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
private:
    sem_t myHandle;
};


class PeriodicThread : public Thread {
protected:
    explicit PeriodicThread (time_t period);
    virtual void periodicActivation () {}
    friend void periodicWrapper(void* time);
};



class Console {
public:
    static char getc ();
    static void putc (char);
};




#endif //PROJECT_BASE_V1_1_SYSCALL_CPP_HPP
