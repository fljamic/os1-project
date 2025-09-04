#ifndef TIMERTHREAD_HPP
#define TIMERTHREAD_HPP

#include "../h/syscall_cpp.hpp"

class TimerThread : public Thread {
    TimerThread(time_t time, thread_t t, sem_t sem)
    : Thread(), sleepTime(time), nit(t), semafor(sem), finished(false) {}

    void run() override {
        time_sleep(sleepTime);
        if (nit->attachedTimer == this)
            semafor->signalThread(nit);
        finished = true;
    }

    time_t sleepTime;
    thread_t nit;
    sem_t semafor;
    bool finished;

    friend class KernelSemaphore;
};

#endif
