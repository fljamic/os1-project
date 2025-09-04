#ifndef KERNELSEMAPHORE_HPP
#define KERNELSEMAPHORE_HPP

#include "../h/List.hpp"
#include "../h/PCB.hpp"
#include "../h/syscall_c.h"

class KernelSemaphore {
public:
    static KernelSemaphore* createSemaphore(unsigned init = 1);

    ~KernelSemaphore();

    void wait();
    void signal();

    void signalThread(thread_t nit);

    int timedWait(time_t timeout);

    int value() const { return val; }

protected:
    void block();
    void unblock();

private:
    explicit KernelSemaphore(unsigned init = 1) : val(init) {}

    int val;
    List<PCB> blockedQueue;

    friend class TimerThread;
};

#endif
