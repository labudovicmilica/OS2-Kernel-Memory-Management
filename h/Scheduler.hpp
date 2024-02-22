//
// Created by os on 8/1/22.
//

#ifndef PROJECT_BASE_V1_1_SCHEDULER_HPP
#define PROJECT_BASE_V1_1_SCHEDULER_HPP

#include "../h/Queue.hpp"
#include "Cache.hpp"

class TCB;


class Scheduler {
    static Queue<TCB> *readyQueue;

    Scheduler() = default;

    ~Scheduler() {
        delete readyQueue;
    }


public:
    Scheduler(const Scheduler&) = delete;
    Scheduler(Scheduler&&) = delete;
    Scheduler operator=(const Scheduler&) = delete;
    Scheduler operator=(Scheduler&&) = delete;

    static Scheduler* getInstance() {
        static Scheduler* scheduler;
        static bool flag = false;
        if (!flag) {
            scheduler = new Scheduler();
            readyQueue = new Queue<TCB>;

            flag = true;
        }
        return scheduler;
    }

    static TCB* get();
    static void put(TCB* tcb);
    static bool empty();

    static Cache* schedulerCache;

    void* operator new(uint64 n) {
        if(schedulerCache == nullptr) {
            schedulerCache = new Cache("Sched cache\0", n, nullptr, nullptr);
        }
        return schedulerCache->alloc();
    }

    void operator delete(void* p) {
        schedulerCache->free(p);
    }

};


#endif //PROJECT_BASE_V1_1_SCHEDULER_HPP
