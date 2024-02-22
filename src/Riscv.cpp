//
// Created by os on 8/1/22.
//

#include "../h/Riscv.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/TCB.hpp"
#include "../h/Sem.hpp"
#include "../lib/console.h"
#include "../h/print.hpp"

void Riscv::popSppSpie() {
    __asm__ volatile ("csrw sepc, ra");
    mc_sstatus(SSTATUS_SPP);
    __asm__ volatile ("sret");
}

void Riscv::syscallHandle() {

    uint64 identifier;
    void* a4;
    __asm__ volatile("mv %0, a0" : "=r" (identifier));
    __asm__ volatile("mv %0, a4" : "=r" (a4) : : "a1", "a2", "a3", "a4");


    switch (identifier) {
        case 0x01:
            //void* mem_alloc(size_t size);
        {
            uint64 arg1;
            __asm__ volatile("mv %0, a1" : "=r" (arg1));
            void *a0 = MemoryAllocator::mem_alloc(arg1);
            __asm__ volatile("mv a0, %0" :: "r" (a0));
        }
            break;
        case 0x02:
            //int mem_free(void* p);
        {
            void* arg1;
            __asm__ volatile("mv %0, a1" : "=r" (arg1));
            int a0 = MemoryAllocator::mem_free(arg1);
            __asm__ volatile("mv a0, %0" :: "r" (a0));
        }
            break;
        case 0x11:
            //int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg, void* stack_space);
        {
            TCB** arg1;
            TCB::Body arg2;
            void* arg3;
            void* arg4 = a4;
            __asm__ volatile("mv %0, a1" : "=r" (arg1));
            __asm__ volatile("mv %0, a2" : "=r" (arg2));
            __asm__ volatile("mv %0, a3" : "=r" (arg3));
            //__asm__ volatile("mv %0, a4" : "=r" (arg4));


            int a0 = TCB::create_thread(arg1, arg2, arg3,arg4);
            //TCB::yield();

            __asm__ volatile("mv a0, %0" :: "r" (a0));

        }
            break;
        case 0x12:
            //int thread_exit();
        {
            int a0 = TCB::running->thread_exit();
            __asm__ volatile("mv a0, %0"::"r" (a0));
        }
            break;
        case 0x13:
            //void thread_dispatch();
            TCB::yield();
            break;
        case 0x14:
            //int thread_start(thread_t* handle);
        {
            TCB *arg1;

            __asm__ volatile("mv %0, a1" : "=r" (arg1));
            int a0 = arg1->start();
            //TCB::yield();

            __asm__ volatile("mv a0, %0" :: "r" (a0));
        }
            break;
        case 0x15:
            //int thread_create_only(thread_t* handle, void (*start_routine)(void*), void* arg, void* stack_space);
        {
            TCB** arg1;
            TCB::Body arg2;
            void* arg3;
            void* arg4 = a4;
            __asm__ volatile("mv %0, a1" : "=r" (arg1));
            __asm__ volatile("mv %0, a2" : "=r" (arg2));
            __asm__ volatile("mv %0, a3" : "=r" (arg3));
            //__asm__ volatile("mv %0, a4" : "=r" (arg4));


            int a0 = TCB::create_thread_only(arg1, arg2, arg3,arg4);

            __asm__ volatile("mv a0, %0" :: "r" (a0));

        }
            break;
        case 0x21:
            //int sem_open(sem_t* handle, unsigned int init);
        {
            Sem** arg1;
            uint64 arg2;
            __asm__ volatile("mv %0, a1" : "=r" (arg1));
            __asm__ volatile("mv %0, a2" : "=r" (arg2));

            int a0 = Sem::open_sem(arg1, arg2);

            __asm__ volatile("mv a0, %0" :: "r" (a0));
        }
        break;
        case 0x22:
            //int sem_close(sem_t id);
        {
            Sem* arg1;
            __asm__ volatile("mv %0, a1" : "=r" (arg1));

            int a0 = arg1->close_sem();

            __asm__ volatile("mv a0, %0" :: "r" (a0));
        }
            break;
        case 0x23:
            //int sem_wait(sem_t id);
        {
            Sem* arg1;
            __asm__ volatile("mv %0, a1" : "=r" (arg1));

            int a0 = arg1->wait();

            __asm__ volatile("mv a0, %0" :: "r" (a0));
        }
            break;
        case 0x24:
            //int sem_signal(sem_t id);
        {
            Sem* arg1;
            __asm__ volatile("mv %0, a1" : "=r" (arg1));

            int a0 = arg1->signal();

            __asm__ volatile("mv a0, %0" :: "r" (a0));
        }
            break;
        case 0x31:
        //int time_sleep(time_t time);
        {
            time_t arg1;
            __asm__ volatile("mv %0, a1" : "=r" (arg1));
            int a0 = TCB::sleep(arg1);

            __asm__ volatile("mv a0, %0" :: "r" (a0));

        }
            break;
        case 0x41:
            //char getc();
        {
            int a0 = __getc();
            __asm__ volatile("mv a0, %0" :: "r" (a0));

        }
            break;
        case 0x42:
            //void putc(char);
        {
            char arg1;
            __asm__ volatile("mv %0, a1" : "=r" (arg1));

            __putc(arg1);

        }
            break;
        case 0x0:

            ms_sstatus(SSTATUS_SPP);

            break;
        default:
            printf("Greska");
    }

}


void Riscv::handleSupervisorTrap() {
    uint64 a0, a4;
    __asm__ volatile("mv %0, a0" : "=r" (a0));
    __asm__ volatile("mv %0, a4" : "=r" (a4));
    volatile uint64 scause = r_scause();

    if (scause == 0x0000000000000008UL) {
        //interrupt: no; cause: ecall from U-mode (8)

        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();

        if (a0 == 0) {
            ms_sstatus(SSTATUS_SPP);
            w_sepc(sepc);
            return;
        }
        __asm__ volatile("mv a0, %0" :: "r" (a0));
        __asm__ volatile("mv a4, %0" :: "r" (a4));

        syscallHandle();

        __asm__ volatile("sd x10, 10 * 8(fp)");

        w_sstatus(sstatus);
        w_sepc(sepc);


    }
    else if (scause == 0x0000000000000009UL) {
        //interrupt: no; cause: ecall from S-mode (9)
        //printString("sistem");
//        printString("s");

        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();

        if (a0 == 0) {
            ms_sstatus(SSTATUS_SPP);
            w_sepc(sepc);
            return;
        }

        __asm__ volatile("mv a0, %0" :: "r" (a0));
        __asm__ volatile("mv a4, %0" :: "r" (a4));

        syscallHandle();

        __asm__ volatile("sd x10, 10 * 8(fp)");

        w_sstatus(sstatus);
        w_sepc(sepc);

    }
    else if (scause == 0x8000000000000001UL) {
        //interrupt: yes; cause: software interrupt (timer)
        //printString("timer");

        Periodic* first = (TCB::sleepingQueue)->peek();
        if (first) {
            first->time_left--;
            while(first && first->time_left == 0) {
                Periodic* p = TCB::sleepingQueue->remove();
                TCB* tcb = p->tcb;
                tcb->setBlocked(false);
                Scheduler::put(tcb);
                delete first;
                first = TCB::sleepingQueue->peek();
            }
        }

        TCB::timeSliceCounter++;
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice()) {
            uint64 volatile sepc = r_sepc();
            uint64 volatile sstatus = r_sstatus();
            //printString("timer\n");
            TCB::timeSliceCounter = 0;
            TCB::yield();
            w_sstatus(sstatus);
            w_sepc(sepc);
        }

        mc_sip(SIP_SSIP);
//        printString("timer\n");
    }
    else if (scause == 0x8000000000000009UL) {
        //interrupt: yes; cause: external interrupt
        //printString("console");

        console_handler();

    }
    else {
        printInt(scause);
        __putc('\n');
        printInt(r_sepc());
        __putc('\n');

        printf("else");

    }


}



uint64 Riscv::r_scause() {
    uint64 volatile scause;
    __asm__ volatile("csrr %0, scause" : "=r" (scause));
    return scause;
}

void Riscv::w_scause(uint64 scause) {
    __asm__ volatile("csrw scause, %0" :: "r" (scause));

}

uint64 Riscv::r_sepc() {
    uint64 volatile sepc;
    __asm__ volatile("csrr %0, sepc" : "=r" (sepc));
    return sepc;
}

void Riscv::w_sepc(uint64 sepc) {
    __asm__ volatile("csrw sepc, %0" :: "r" (sepc));
}

uint64 Riscv::r_stvec() {
    uint64 volatile stvec;
    __asm__ volatile("csrr %0, stvec" : "=r" (stvec));
    return stvec;
}

void Riscv::w_stvec(uint64 stvec) {
    __asm__ volatile("csrw stvec, %0" :: "r" (stvec));
}

uint64 Riscv::r_stval() {
    uint64 volatile stval;
    __asm__ volatile("csrr %0, stval" : "=r" (stval));
    return stval;
}

void Riscv::w_stval(uint64 stval) {
    __asm__ volatile("csrw stval, %0" :: "r" (stval));
}

uint64 Riscv::r_sip() {
    uint64 volatile sip;
    __asm__ volatile("csrr %0, sip" : "=r" (sip));
    return sip;
}

void Riscv::w_sip(uint64 sip) {
    __asm__ volatile("csrw sip, %0" :: "r" (sip));
}

void Riscv::ms_sip(uint64 mask) {
    __asm__ volatile("csrs sip, %0" :: "r" (mask));

}

void Riscv::mc_sip(uint64 mask) {
    __asm__ volatile("csrc sip, %0" :: "r" (mask));
}

uint64 Riscv::r_sstatus() {
    uint64 volatile sstatus;
    __asm__ volatile("csrr %0, sstatus" : "=r" (sstatus));
    return sstatus;
}

void Riscv::w_sstatus(uint64 sstatus) {
    __asm__ volatile("csrw sstatus, %0" :: "r" (sstatus));
}

void Riscv::ms_sstatus(uint64 mask) {
    __asm__ volatile("csrs sstatus, %0" :: "r" (mask));
}

void Riscv::mc_sstatus(uint64 mask) {
    __asm__ volatile("csrc sstatus, %0" :: "r" (mask));
}
