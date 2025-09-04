#include "../h/KernelConsole.hpp"

bool KernelConsole::signaled = false;
CharBuffer* KernelConsole::input = nullptr;
CharBuffer* KernelConsole::output = nullptr;

void KernelConsole::initialize() {
    input = new CharBuffer();
    output = new CharBuffer();
}

void KernelConsole::signalFinished() {
    signaled = true;
}

void KernelConsole::consoleHandler() {
    volatile uint8* status = (volatile uint8*)CONSOLE_STATUS;
    if (*status & CONSOLE_RX_STATUS_BIT) {
        volatile char* c = (volatile char*)CONSOLE_RX_DATA;
        input->put(*c);
    }
}

void KernelConsole::consoleThreadBody(void* arg) {
    while (true) {
        if (signaled && output->isEmpty()) thread_exit();

        volatile uint8* status = (volatile uint8*)CONSOLE_STATUS;
        if ((*status & CONSOLE_TX_STATUS_BIT) && !output->isEmpty())
            *((volatile char*)CONSOLE_TX_DATA) = KernelConsole::output->get();
        else
            thread_dispatch();
    }
}
