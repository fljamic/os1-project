#include "../lib/hw.h"
#include "../h/syscall_c.h"
#include "../h/syscall_cpp.hpp"
#include "../h/PCB.hpp"
#include "../h/RiscV.hpp"
#include "../h/KernelConsole.hpp"

extern void userMain();

void userMainWrapper(void* arg) {
    //userMain();
}

void consoleWrapper(void* arg) {
    KernelConsole::consoleThreadBody(arg);
}

void main() {
    RiscV::w_stvec((uint64) &RiscV::interruptRoutine);
    KernelConsole::initialize();

    thread_t mainThread;
    thread_create(&mainThread, nullptr, nullptr);
    PCB::running = mainThread;
    mainThread->setPrivileged(true);

    thread_t consoleThread;
    thread_create(&consoleThread, consoleWrapper, nullptr);

    thread_t userMainThread;
    thread_create(&userMainThread, userMainWrapper, nullptr);

    RiscV::ms_sstatus(RiscV::SSTATUS_SIE);


    while (userMainThread->getState() != PCB::FINISHED)
        thread_dispatch();

    KernelConsole::signalFinished();
    while (consoleThread->getState() != PCB::FINISHED)
        thread_dispatch();

    delete userMainThread;
    delete consoleThread;
    delete mainThread;
}
