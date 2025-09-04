#ifndef CHARBUFFER_HPP
#define CHARBUFFER_HPP

#include "../lib/hw.h"
#include "../h/KernelSemaphore.hpp"

class CharBuffer {
public:
    static const size_t BUFFER_SIZE = 4096;

    CharBuffer();
    ~CharBuffer();

    char get();
    void put(char c);

    bool isFull();
    bool isEmpty();

private:
    char buffer[BUFFER_SIZE];
    size_t head, tail;
    KernelSemaphore* itemAvailable;
};

#endif
