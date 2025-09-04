#ifndef PCB_HPP
#define PCB_HPP

#include "../lib/hw.h"
#include "../h/Scheduler.hpp"
#include "../h/KernelSemaphore.hpp"

class KernelSemaphore;
class TimerThread;

class PCB {
public:
    using Body = void (*)(void*);

    enum State {
        INITALIZED,
        READY,
        RUNNING,
        BLOCKED,
        SLEEPING,
        FINISHED
    };

    static PCB* createPCB(Body body, void* arg, void* stack_space);

    ~PCB() { delete stack; }

    State getState() const { return state; }
    void setState(State value) { state = value; }

    void setPrivileged(bool val) { privileged = val; }

    uint64 getTimeSlice() const { return timeSlice; }

    static PCB* running;

private:
    explicit PCB(Body body, void* arg, void* stack_space);

    struct Context {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    void* argument;
    uint8* stack;
    Context context;
    uint64 timeSlice;
    State state;
    time_t timeSleepCounter;
    bool backFromClosedSemaphore;
    TimerThread* attachedTimer;
    bool privileged = false;

    static uint64 timeSliceCounter;

    static void threadWrapper();

    static void dispatch();

    static void contextSwitch(Context* oldContext, Context* newContext);

    friend class RiscV;
    friend class KernelSemaphore;
    friend class Scheduler;
    friend class TimerThread;
};

#endif
