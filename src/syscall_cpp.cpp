#include "../h/syscall_cpp.hpp"
#include "../h/PCB.hpp"

void* operator new(uint64 n) {
    return mem_alloc(n);
}

void* operator new[](uint64 n) {
    return mem_alloc(n);
}

void operator delete(void* ptr) {
    mem_free(ptr);
}

void operator delete[](void* ptr) {
    mem_free(ptr);
}

Thread::Thread(void (*body)(void *), void *arg) : body(body), arg(arg) {}

Thread::~Thread() {
    if (myHandle) delete myHandle;
}

int Thread::start() {
    thread_create(&myHandle, body, arg);
    return 0;
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t t) {
    return time_sleep(t);
}

void Thread::runWrapper(void* arg) {
    ((Thread*)arg)->run();
}

Thread::Thread() : body(runWrapper), arg(this) {}

Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

int Semaphore::timedWait(time_t t) {
    return sem_timedwait(myHandle, t);
}

int Semaphore::tryWait() {
    return sem_trywait(myHandle);
}

void PeriodicThread::terminate() {
    terminated = true;
}

PeriodicThread::PeriodicThread(time_t period)
    : Thread(PeriodicThread::runWrapper, this), period(period), terminated(false) {}

void PeriodicThread::runWrapper(void* arg) {
    PeriodicThread* thread = (PeriodicThread*)arg;

    while (!(thread->terminated)) {
        thread->periodicActivation();
        sleep(thread->period);
    }
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}
