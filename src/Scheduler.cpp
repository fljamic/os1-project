#include "../h/Scheduler.hpp"
#include "../h/PCB.hpp"

List<PCB> Scheduler::processQueue;
List<PCB> Scheduler::sleepingQueue;

PCB* Scheduler::get() {
    return processQueue.removeFirst();
}

void Scheduler::put(PCB* pcb) {
    pcb->setState(PCB::READY);
    processQueue.addLast(pcb);
}

PCB* Scheduler::getFromSleep() {
    return sleepingQueue.removeFirst();
}

void Scheduler::putToSleep(PCB* pcb) {
    pcb->setState(PCB::SLEEPING);
    sleepingQueue.addLast(pcb);
}

void Scheduler::tickSleeping() {
    for (uint64 i = 0; i < sleepingQueue.getSize(); i++) {
        PCB* sleeping = getFromSleep();
        if (--(sleeping->timeSleepCounter) <= 0)
            put(sleeping);
        else
            putToSleep(sleeping);
    }
}
