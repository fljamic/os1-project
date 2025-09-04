#ifndef KERNELCONSOLE_HPP
#define KERNELCONSOLE_HPP

#include "../lib/hw.h"
#include "../h/CharBuffer.hpp"
#include "../h/RiscV.hpp"

class KernelConsole {
public:
    static void initialize();

    static void signalFinished();

    static void consoleHandler();
    static void consoleThreadBody(void*);

private:
    static bool signaled;

    static CharBuffer* input;
    static CharBuffer* output;

    friend class RiscV;
};

#endif
