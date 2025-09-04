#ifndef RISCV_HPP
#define RISCV_HPP

#include "../lib/hw.h"

class RiscV {
public:
    static void popSppSie(); // ne moze da bude inline

    static void terminate();

    static uint64 r_scause();
    static void w_scause(uint64 scause);

    static uint64 r_sepc();
    static void w_sepc(uint64 sepc);

    static uint64 r_stvec();
    static void w_stvec(uint64 stvec);

    static uint64 r_stval();
    static void w_stval(uint64 stval);

    enum BitMaskSip {
        SIP_SSIP = (1 << 1),
        SIP_STIP = (1 << 5),
        SIP_SEIP = (1 << 9)
    };

    static void ms_sip(uint64 mask); // mask set
    static void mc_sip(uint64 mask); // mask clear

    static uint64 r_sip();
    static void w_sip(uint64 sip);

    enum BitMaskSstatus {
        SSTATUS_SIE = (1 << 1),
        SSTATUS_SPIE = (1 << 5),
        SSTATUS_SPP = (1 << 8)
    };

    static void ms_sstatus(uint64 mask);
    static void mc_sstatus(uint64 mask);

    static uint64 r_sstatus();
    static void w_sstatus(uint64 sstatus);

    static void interruptRoutine();

private:
    static void interruptHandler();
};

inline void RiscV::terminate() {
    uint32* adr = (uint32*)0x100000;
    *adr = 0x5555;
}

inline uint64 RiscV::r_scause() {
    uint64 volatile scause;
    __asm__ volatile ("csrr %0, scause" : "=r" (scause));
    return scause;
}

inline void RiscV::w_scause(uint64 scause) {
    __asm__ volatile ("csrw scause, %0" : : "r" (scause));
}

inline uint64 RiscV::r_sepc() {
    uint64 volatile sepc;
    __asm__ volatile ("csrr %0, sepc" : "=r" (sepc));
    return sepc;
}

inline void RiscV::w_sepc(uint64 sepc) {
    __asm__ volatile ("csrw sepc, %0" : : "r" (sepc));
}

inline uint64 RiscV::r_stvec() {
    uint64 volatile stvec;
    __asm__ volatile ("csrr %0, stvec" : "=r" (stvec));
    return stvec;
}

inline void RiscV::w_stvec(uint64 stvec) {
    __asm__ volatile ("csrw stvec, %0" : : "r" (stvec));
}

inline uint64 RiscV::r_stval() {
    uint64 volatile stval;
    __asm__ volatile ("csrr %0, stval" : "=r" (stval));
    return stval;
}

inline void RiscV::w_stval(uint64 stval) {
    __asm__ volatile ("csrw stval, %0" : : "r" (stval));
}

inline void RiscV::ms_sip(uint64 mask) {
    __asm__ volatile ("csrs sip, %0" : : "r" (mask));
}

inline void RiscV::mc_sip(uint64 mask) {
    __asm__ volatile ("csrc sip, %0" : : "r" (mask));
}

inline uint64 RiscV::r_sip() {
    uint64 volatile sip;
    __asm__ volatile ("csrr %0, sip" : "=r" (sip));
    return sip;
}

inline void RiscV::w_sip(uint64 sip) {
    __asm__ volatile ("csrw sip, %0" : : "r" (sip));
}

inline void RiscV::ms_sstatus(uint64 mask) {
    __asm__ volatile ("csrs sstatus, %0" : : "r" (mask));
}

inline void RiscV::mc_sstatus(uint64 mask) {
    __asm__ volatile ("csrc sstatus, %0" : : "r" (mask));
}

inline uint64 RiscV::r_sstatus() {
    uint64 volatile sstatus;
    __asm__ volatile ("csrr %0, sstatus" : "=r" (sstatus));
    return sstatus;
}

inline void RiscV::w_sstatus(uint64 sstatus) {
    __asm__ volatile ("csrw sstatus, %0" : : "r" (sstatus));
}

#endif
