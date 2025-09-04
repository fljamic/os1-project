#include "../h/PCB.hpp"
#include "../h/RiscV.hpp"

PCB* PCB::running = nullptr;
uint64 PCB::timeSliceCounter = 0;

PCB::PCB(Body body, void* arg, void* stack_space) :
        body(body),
        argument(arg),
        stack(body ? (uint8*)stack_space : 0),
        context({
            (uint64)&threadWrapper,
            stack ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0 // stack je najniza (poslednja) adresa steka, pa stavljamo sp na najvisu tj. prvu
                }),
        timeSlice(DEFAULT_TIME_SLICE),
        state(INITALIZED),
        timeSleepCounter(0),
        backFromClosedSemaphore(false),
        attachedTimer(nullptr) {
    if (body)
        Scheduler::put(this);
    else
        this->state = RUNNING;
}

PCB* PCB::createPCB(Body body, void* arg, void* stack_space) {
    return new PCB(body, arg, stack_space);
}

void PCB::dispatch() {
    PCB::timeSliceCounter = 0;
    PCB* old = running;
    if (old->getState() == RUNNING) Scheduler::put(old);
    running = Scheduler::get();
    running->setState(RUNNING);

    PCB::contextSwitch(&old->context, &running->context);
}

void PCB::threadWrapper() {
    RiscV::popSppSie();
    running->body(running->argument);
    thread_exit();
}