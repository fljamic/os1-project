#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "../h/List.hpp"

class PCB;

class Scheduler {
private:
    static List<PCB> processQueue;
    static List<PCB> sleepingQueue;

public:
    static PCB* get();
    static void put(PCB* pcb);

    static PCB* getFromSleep();
    static void putToSleep(PCB* pcb);

    static void tickSleeping();
};

#endif
