//
// Created by os on 8/1/22.
//
#include "../h/Scheduler.hpp"

Queue<TCB> *Scheduler::readyQueue;
Cache* Scheduler::schedulerCache = nullptr;
TCB *Scheduler::get() {
    getInstance();
    return readyQueue->remove();
}

void Scheduler::put(TCB *tcb) {
    getInstance();
    readyQueue->add(tcb);
}

bool Scheduler::empty() {
    getInstance();
    if (readyQueue->peek() == nullptr) return true;
    else return false;
}

