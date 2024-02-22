//
// Created by os on 7/31/22.
//

#ifndef PROJECT_BASE_V1_1_TCB_HPP
#define PROJECT_BASE_V1_1_TCB_HPP


#include "../lib/hw.h"
#include "Scheduler.hpp"
#include "MemoryAllocator.hpp"
#include "Cache.hpp"

struct Periodic {
    TCB* tcb;
    time_t time_left;

    Periodic(TCB* tcb, time_t time) : tcb(tcb), time_left(time) {}

    static Cache* periodicCache;

    void* operator new(uint64 n) {
        if(periodicCache == nullptr) {
            periodicCache = new Cache("Period cache\0", n, nullptr, nullptr);
        }
        return periodicCache->alloc();
    }


    void operator delete(void* p) {
        periodicCache->free(p);
    }

};

class TCB{

public:
    ~TCB() {
        delete stack;
    }

    static Cache* tcbCache;


    bool isFinished() const;
    void setFinished(bool finished);

    bool isBlocked() const;
    void setBlocked(bool blocked);

    uint64 getTimeSlice() const;

    using Body = void (*)(void*);

    static int create_thread(TCB** myHandle, TCB::Body body, void* arg, void* stack_space);
    static int create_thread_only(TCB** myHandle, TCB::Body body, void* arg, void* stack_space);
    int thread_exit();
    int start();
    static void yield();

    static TCB* running;

    TCB(const TCB&) = delete;
    TCB& operator=(const TCB&) = delete;
    static int sleep(time_t time);
    static Queue<Periodic> *sleepingQueue;
    static Cache* stackCache;
private:
    TCB(Body body, void* arg, void* stack_space) : body(body), arg(arg), stack((uint64*)stack_space),
                     context({(uint64) &threadWrapper, (uint64 )&stack[STACK_SIZE]}),
                     finished(false), blocked(false), idle(false), timeSlice(DEFAULT_TIME_SLICE) { }

    struct Context{
        uint64 ra;
        uint64 sp;
    };
    static bool madeSleep;
    static TCB* idleThread;
    Body body;
    void* arg;
    uint64* stack;
    Context context;
    bool finished;
    bool blocked;
    bool idle;


    uint64 timeSlice;
    static uint64 timeSliceCounter;

    friend class Riscv;

    static void contextSwitch(Context* oldContext, Context* runningContext);

    static void threadWrapper();

    static void dispatch();

    static uint64 constexpr STACK_SIZE = 2*DEFAULT_STACK_SIZE/ sizeof(size_t);

public:
    void* operator new(uint64 n) {
        if(tcbCache == nullptr) {
            tcbCache = new Cache("TCB cache\0", n, nullptr, nullptr);
        }
        return tcbCache->alloc();
    }

    void* operator new[](uint64 n) {
        if(tcbCache == nullptr) {
            tcbCache = new Cache("TCB cache\0", n, nullptr, nullptr);
        }
        return tcbCache->alloc();
    }

    void operator delete(void* p) {
        tcbCache->free(p);
    }

    void operator delete[](void* p) {
        tcbCache->free(p);
    }

};


#endif //PROJECT_BASE_V1_1_TCB_HPP
