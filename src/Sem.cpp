//
// Created by os on 8/9/22.
//

#include "../h/Sem.hpp"
#include "../h/TCB.hpp"

Cache* Sem::semCache = nullptr;

int Sem::value() const {
    return val;
}

int Sem::wait() {
//    printString("wait\n");
    if (--val < 0) {
        //printString("wait\n");
        block();
    }
    if (interrupted) return -1;
    else return 0;
}

int Sem::signal() {
//    printString("signal\n");
    if (++val <= 0) unblock();
    return 0;
}

void Sem::block() {
//    printString("block\n");
    TCB* old = TCB::running;
    old->setBlocked(true);
    blockedQueue->add(old);
    TCB::yield();
}

void Sem::unblock() {
//    printString("unblock\n");
    TCB* unblocked = blockedQueue->remove();
    if (unblocked) {
        unblocked->setBlocked(false);
        Scheduler::put(unblocked);
    }
    //TCB::yield();

}

uint64 Sem::open_sem(Sem **handle, uint64 init) {
    Sem* sem = new Sem(init);
    if (!sem) return -1;
    *handle = sem;
    return 0;
}

uint64 Sem::close_sem() {
    TCB *old = nullptr;
    interrupted = true;
    while (blockedQueue->peek()) {
        old = blockedQueue->remove();
        old->setBlocked(false);
        Scheduler::put(old);
    }

    //treba da se odblokiraju blokirane niti i da njihov wait vrati gresku
    return 0;
}


