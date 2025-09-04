#include "../h/CharBuffer.hpp"

CharBuffer::CharBuffer() : head(0), tail(0), itemAvailable(KernelSemaphore::createSemaphore(0)) {}

CharBuffer::~CharBuffer() { delete itemAvailable; }

char CharBuffer::get() {
    itemAvailable->wait();
    if (isEmpty()) return -1;
    char c = buffer[head];
    head = (head + 1) % BUFFER_SIZE;
    return c;
}

void CharBuffer::put(char c) {
    if (!isFull()) {
        buffer[tail] = c;
        tail = (tail + 1) % BUFFER_SIZE;
        itemAvailable->signal();
    }
}

bool CharBuffer::isFull() {
    return (tail + 1) % BUFFER_SIZE == head;
}

bool CharBuffer::isEmpty() {
    return head == tail;
}
