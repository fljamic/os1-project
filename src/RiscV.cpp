#include "../h/RiscV.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/syscall_c.h"
#include "../h/KernelSemaphore.hpp"
#include "../h/PCB.hpp"
#include "../h/KernelConsole.hpp"

static const uint64 CAUSE_TIMER                 = 0x8000000000000001UL; // supervizor sw prekid
static const uint64 CAUSE_CONSOLE               = 0x8000000000000009UL; // spoljasnji hw prekid
static const uint64 CAUSE_ILLEGAL_INSTRUCTION   = 0x2UL;                // ilegalna instrukcija
static const uint64 CAUSE_ILLEGAL_READ          = 0x5UL;                // nedozvoljena adresa citanja
static const uint64 CAUSE_ILLEGAL_WRITE         = 0x7UL;                // nedozvoljena adresa upisa
static const uint64 CAUSE_ECALL_USER            = 0x8UL;                // ecall iz korisnickog rezima
static const uint64 CAUSE_ECALL_SUPERVISOR      = 0x9UL;                // ecall iz sistemskog rezima

void RiscV::popSppSie() { // ovo ce sret uzeti iz sstatus
    if (PCB::running->privileged) ms_sstatus(SSTATUS_SPP);
    else mc_sstatus(SSTATUS_SPP);
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void RiscV::interruptHandler() {
    uint64 scause = r_scause();
    switch (scause) {
        case CAUSE_ECALL_SUPERVISOR:
        case CAUSE_ECALL_USER: {
            volatile uint64 sepc = r_sepc() + 4; // da se sa ecall prebaci na sledecu instrukciju
            volatile uint64 sstatus = r_sstatus();

            volatile uint64 base;
            __asm__ volatile ("mv %0, fp" : "=r" (base));
            uint64 a[5];
            for (uint64 i = 0; i < 5; i++)
                __asm__ volatile ("ld %0, 0(%1)" : "=r" (a[i]) : "r" ((uint64)(base + 80 + i*8)));

            switch (a[0]) {
                case CODE_MEM_ALLOC: {
                    void* ret = MemoryAllocator::mem_alloc(a[1]*MEM_BLOCK_SIZE);
                    __asm__ volatile ("sd %0, 10*8(fp)" : : "r" (ret));
                    break;
                } case CODE_MEM_FREE: {
                    int status = MemoryAllocator::mem_free((void*)a[1]);
                    __asm__ volatile ("sd %0, 10*8(fp)" : : "r" (status));
                    break;
                } case CODE_THREAD_CREATE: {
                    int status = -1;
                    thread_t* handle = (thread_t*)a[1];
                    if (handle) {
                        *handle = PCB::createPCB((PCB::Body)a[2], (void*)a[3], (a[2] ? (void*)a[4] : 0));
                        status = (*handle ? 0 : -2);
                    }
                    __asm__ volatile ("sd %0, 10*8(fp)" : : "r" (status));
                    break;
                } case CODE_THREAD_EXIT: {
                    int status = -1;
                    if (PCB::running->getState() != PCB::FINISHED) {
                        status = 0;
                        PCB::running->setState(PCB::FINISHED);
                        PCB::dispatch();
                    }
                    __asm__ volatile ("sd %0, 10*8(fp)" : : "r" (status));
                    break;
                } case CODE_THREAD_DISPATCH: {
                    PCB::dispatch();
                    break;
                } case CODE_SEM_OPEN: {
                    int status = -1;
                    sem_t* handle = (sem_t*)a[1];
                    if (handle) {
                        *handle = KernelSemaphore::createSemaphore((unsigned)a[2]);
                        status = (*handle ? 0 : -2);
                    }
                    __asm__ volatile ("sd %0, 10*8(fp)" : : "r" (status));
                    break;
                } case CODE_SEM_CLOSE: {
                    int status = 0;
                    sem_t handle = (sem_t)a[1];
                    if (handle) delete handle;
                    else status = -1;
                    __asm__ volatile ("sd %0, 10*8(fp)" : : "r" (status));
                    break;
                } case CODE_SEM_WAIT: {
                    int status = -2;
                    sem_t id = (sem_t)a[1];
                    if (id) {
                        status = 0;
                        id->wait();
                        if (PCB::running->backFromClosedSemaphore) status = -1;
                    }
                    __asm__ volatile ("sd %0, 10*8(fp)" : : "r" (status));
                    break;
                } case CODE_SEM_SIGNAL: {
                    int status = 0;
                    sem_t id = (sem_t)a[1];
                    if (id) id->signal();
                    else status = -1;
                    __asm__ volatile ("sd %0, 10*8(fp)" : : "r" (status));
                    break;
                } case CODE_SEM_TIMEDWAIT: {
                    sem_t id = (sem_t)a[1];
                    time_t timeout = (time_t)a[2];
                    int status = -3; // handle == nullptr
                    if (id) status = id->timedWait(timeout);
                    __asm__ volatile ("sd %0, 10*8(fp)" : : "r" (status));
                    break;
                } case CODE_SEM_TRYWAIT: {
                    sem_t id = (sem_t)a[1];
                    int status = -1;
                    if (id) {
                        if (id->value() > 0) id->wait();
                        status = (id->value() > 0 ? 1 : 0);
                    }
                    __asm__ volatile ("sd %0, 10*8(fp)" : : "r" (status));
                    break;
                } case CODE_TIME_SLEEP: {
                    int status = -1;
                    if (a[1] > 0) {
                        status = 0;
                        PCB::running->timeSleepCounter = a[1];
                        Scheduler::putToSleep(PCB::running);
                        PCB::dispatch();
                    }
                    __asm__ volatile ("sd %0, 10*8(fp)" : : "r" (status));
                    break;
                } case CODE_GETC: {
                    char c = KernelConsole::input->get();
                    KernelConsole::output->put(c); // da bi se prikazivali karakteri koji se kucaju
                    __asm__ volatile ("sd %0, 10*8(fp)" : : "r" (c));
                    break;
                } case CODE_PUTC: {
                    KernelConsole::output->put((char)a[1]);
                    break;
                }
            }

            w_sstatus(sstatus);
            w_sepc(sepc);
            break;
        } case CAUSE_TIMER: {
            volatile uint64 sepc = r_sepc();
            volatile uint64 sstatus = r_sstatus();

            Scheduler::tickSleeping();

            if (++PCB::timeSliceCounter >= PCB::running->getTimeSlice())
                PCB::dispatch();

            w_sstatus(sstatus);
            w_sepc(sepc);
            mc_sip(SIP_SSIP);
            break;
        } case CAUSE_CONSOLE: {
            if (plic_claim() == 10) {
                KernelConsole::consoleHandler();
                plic_complete(10);
            }
            break;
        } case CAUSE_ILLEGAL_INSTRUCTION: {
            for(char ch : "Illegal instruction\n")
                *((volatile char*)CONSOLE_TX_DATA) = ch;
            terminate();
            break;
        } case CAUSE_ILLEGAL_READ: {
            for(char ch : "Illegal read\n")
                *((volatile char*)CONSOLE_TX_DATA) = ch;
            terminate();
            break;
        } case CAUSE_ILLEGAL_WRITE: {
            for(char ch : "Illegal write\n")
                *((volatile char*)CONSOLE_TX_DATA) = ch;
            terminate();
            break;
        } default: {
            for(char ch : "Unhandled case\n")
                *((volatile char*)CONSOLE_TX_DATA) = ch;
            terminate();
            break;
        }
    }
}
