//
// Created by os on 8/1/22.
//

#ifndef PROJECT_BASE_V1_1_RISCV_HPP
#define PROJECT_BASE_V1_1_RISCV_HPP

class TCB;
#include "../lib/hw.h"

class Riscv {
    static void handleSupervisorTrap();

    static void syscallHandle();
public:

    static void popSppSpie();

    static void supervisorTrap();

    //push registers x3,...,x31 on stack
    static void pushRegisters();

    //pop registers x3,...,x31 from stack
    static void popRegisters();

    friend TCB;

    //read scause register
    static uint64 r_scause();

    //write scause register
    static void w_scause(uint64 scause);

    //read sepc register
    static uint64 r_sepc();

    //write sepc register
    static void w_sepc(uint64 sepc);

    //read stvec register
    static uint64 r_stvec();

    //write stvec register
    static void w_stvec(uint64 stvec);

    //read stval register
    static uint64 r_stval();

    //write stval register
    static void w_stval(uint64 stval);

    //sip = supervisor interrupt pending
    //SSIP - postoji zahtev za softverski prekid
    enum BitMaskSip{
        SIP_SSIP = (1 << 1),
        SIP_STIE = (1 << 5),
        SIP_SEIP = (1 << 9)
    };

    //read sip register
    static uint64 r_sip();

    //write sip register
    static void w_sip(uint64 sip);

    //mask set sip register
    static void ms_sip(uint64 mask);

    //mask clear sip register
    static void mc_sip(uint64 mask);


    //sstatus - supervisor status
    //sie - supervisor interrupt enable
    //spie - supervisor previous interrupt enable
    //spp - supervisor previous privilege
    enum BitMaskSstatus{
        SSTATUS_SIE = (1 << 1),
        SSTATUS_SPIE = (1 << 5),
        SSTATUS_SPP = (1 << 8)
    };

    //read sstatus register
    static uint64 r_sstatus();

    //write sstatus register
    static void w_sstatus(uint64 sstatus);

    //mask set sstatus register
    static void ms_sstatus(uint64 mask);

    //mask clear sstatus register
    static void mc_sstatus(uint64 mask);


};


#endif //PROJECT_BASE_V1_1_RISCV_HPP
