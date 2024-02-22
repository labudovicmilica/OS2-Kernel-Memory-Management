//
// Created by os on 7/31/22.
//

#include "../h/TCB.hpp"
#include "../h/Riscv.hpp"
#include "../h/print.hpp"

Cache* Periodic::periodicCache = nullptr;

bool TCB::madeSleep = false;
TCB* TCB::idleThread = nullptr;
Queue<Periodic>* TCB::sleepingQueue = new Queue<Periodic>;
TCB* TCB::running = nullptr;
uint64 TCB::timeSliceCounter = 0;
Cache* TCB::tcbCache = nullptr;
Cache* TCB::stackCache = nullptr;

void idleFunction(void* arg) {
    while (1) {
        //printString("idle\n");
    }
}

int TCB::create_thread(TCB** myHandle, TCB::Body body, void* arg, void* stack_space) {
    TCB* tcb = new TCB(body, arg, stack_space);
    if (!tcb) return -1;
    tcb->start();
    *myHandle = tcb;
    if (!madeSleep) {
        sleepingQueue = new Queue<Periodic>;
        madeSleep = true;
    }
    static bool oneIdle = true;
    if (oneIdle) {
        idleThread = new TCB(idleFunction, nullptr, MemoryAllocator::mem_alloc(STACK_SIZE/MEM_BLOCK_SIZE));
        idleThread->idle = true;
    }
//    printString("tcb->create\n");
    return 0;
}

int TCB::create_thread_only(TCB** myHandle, TCB::Body body, void *arg, void *stack_space) {
    TCB* tcb = new TCB(body, arg, stack_space);
    if (!tcb) return -1;
    *myHandle = tcb;
    if (!madeSleep) {
        sleepingQueue = new Queue<Periodic>;
        madeSleep = true;
    }
    return 0;
}

int TCB::start() {
    if (body != nullptr) Scheduler::put(this);
    //printString("tcb->start\n");
    return 0;
}

bool TCB::isFinished() const {
    return finished;
}

void TCB::setFinished(bool finished) {
    this->finished = finished;
}

void TCB::yield() {
    if (running) Riscv::pushRegisters();

    dispatch();
    timeSliceCounter = 0;

    if (running) Riscv::popRegisters();


}

void TCB::dispatch() {
    TCB* old = running;
    //printString("tcb->dispatch");
    bool empty = false;
    if (Scheduler::empty()) empty = true;
    if (!old->finished && !old->blocked && !old->idle) Scheduler::put(old);
    if (empty) running = idleThread;
    else running = Scheduler::get();
    if (running ) contextSwitch(&old->context, &running->context);
}

int TCB::thread_exit() {
//    printString("exit\n");
    setFinished(true);
    __asm__ volatile("li a0, 0x13");
    __asm__ volatile("ecall");
    return 0;
}

void TCB::threadWrapper() {
    Riscv::popSppSpie();
    running->body(running->arg);
    running->thread_exit();
}

bool TCB::isBlocked() const {
    return blocked;
}

void TCB::setBlocked(bool blocked) {
    TCB::blocked = blocked;
}

uint64 TCB::getTimeSlice() const {
    return timeSlice;
}

int TCB::sleep(time_t time) {
    if (time < 0) return -1;
    Periodic* elem = new Periodic(running, time);
    Periodic* current = sleepingQueue->peek(), *previous = nullptr;
    if (current == nullptr) {
        sleepingQueue->add(elem);
        //Periodic* next = sleepingQueue->next(elem);
        //if (next) next->time_left -= elem->time_left;
    }
    else if(time == 0) {
        yield();
        return 0;
    }
    else if(current->time_left > elem->time_left) {
        sleepingQueue->addFirst(elem);
        Periodic* next = sleepingQueue->next(elem);
        next->time_left -= elem->time_left;
    }
    else {
        while (current && elem->time_left >= current->time_left) {
            elem->time_left -= current->time_left;
            previous = current;
            current = sleepingQueue->next(current);
        }
        sleepingQueue->addAfter(previous, elem);
        Periodic* next = sleepingQueue->next(elem);
        if (next) next->time_left -= elem->time_left;
    }
    running->setBlocked(true);
    yield();
    return 0;
}

