#include "../h/KernelSemaphore.hpp"
#include "../h/TimerThread.hpp"

KernelSemaphore* KernelSemaphore::createSemaphore(unsigned init) {
    return new KernelSemaphore(init);
}

KernelSemaphore::~KernelSemaphore() {
    PCB* pcb = blockedQueue.removeFirst();
    while (pcb) {
        pcb->backFromClosedSemaphore = true;
        Scheduler::put(pcb);
        pcb = blockedQueue.removeFirst();
    }
}

void KernelSemaphore::block() {
    blockedQueue.addLast(PCB::running);
    PCB::running->setState(PCB::BLOCKED);
    thread_dispatch();
}

void KernelSemaphore::unblock() {
    PCB* pcb = blockedQueue.removeFirst();
    if (pcb) {
        pcb->backFromClosedSemaphore = false;
        Scheduler::put(pcb);
    }
}

void KernelSemaphore::wait() {
    if (--val < 0)
        block();
}

void KernelSemaphore::signal() {
    if (++val <= 0)
        unblock();
}

void KernelSemaphore::signalThread(thread_t nit) {
    ++val;
    for (uint64 i = 0; i < blockedQueue.getSize(); i++) {
        PCB* pcb = blockedQueue.removeFirst();
        if (pcb == nit) {
            pcb->backFromClosedSemaphore = false;
            Scheduler::put(pcb);
        }
        else blockedQueue.addLast(pcb);
    }
}

int KernelSemaphore::timedWait(time_t timeout) {
    if (--val < 0) {
        TimerThread* tajmer = new TimerThread(timeout, PCB::running, this);
        PCB::running->attachedTimer = tajmer;
        tajmer->start();
        block();
        PCB::running->attachedTimer = nullptr;
        if (PCB::running->backFromClosedSemaphore) {
            delete tajmer;
            return -1; // SEMDEAD
        }
        if (tajmer->finished) {
            delete tajmer;
            return -2; // SEMTIMEOUT
        }
        delete tajmer;
    }
    return 0; // OK
}
