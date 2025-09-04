#include "../h/syscall_c.h"

void* mem_alloc(size_t size) {
    size_t blocks = (size + MEM_BLOCK_SIZE - 1)/MEM_BLOCK_SIZE;
    __asm__ volatile ("mv a1, %0" : : "r" (blocks));
    __asm__ volatile ("mv a0, %0" : : "r" (CODE_MEM_ALLOC));
    __asm__ volatile ("ecall");

    void* adr;
    __asm__ volatile ("mv %0, a0" : "=r" (adr));
    return adr;
}
int mem_free(void* ptr) {
    __asm__ volatile ("mv a1, %0" : : "r" (ptr));
    __asm__ volatile ("mv a0, %0" : : "r" (CODE_MEM_FREE));
    __asm__ volatile ("ecall");

    int status;
    __asm__ volatile ("mv %0, a0" : "=r" (status));
    return status;
}

int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg) {
    void* stack_space = 0;
    if (start_routine) {
        stack_space = mem_alloc(DEFAULT_STACK_SIZE);
        if (!stack_space) return -3; // stack allocation failed
    }
    __asm__ volatile ("mv a4, %0" : : "r" (stack_space));
    __asm__ volatile ("mv a3, %0" : : "r" (arg));
    __asm__ volatile ("mv a2, %0" : : "r" (start_routine));
    __asm__ volatile ("mv a1, %0" : : "r" (handle));
    __asm__ volatile ("mv a0, %0" : : "r" (CODE_THREAD_CREATE));
    __asm__ volatile ("ecall");

    int status;
    __asm__ volatile ("mv %0, a0" : "=r" (status));
    return status;
}
int thread_exit() {
    __asm__ volatile ("mv a0, %0" : : "r" (CODE_THREAD_EXIT));
    __asm__ volatile ("ecall");

    int status;
    __asm__ volatile ("mv %0, a0" : "=r" (status));
    return status;
}
void thread_dispatch() {
    __asm__ volatile ("mv a0, %0" : : "r" (CODE_THREAD_DISPATCH));
    __asm__ volatile ("ecall");
}

int sem_open(sem_t* handle, unsigned init) {
    __asm__ volatile ("mv a2, %0" : : "r" (init));
    __asm__ volatile ("mv a1, %0" : : "r" (handle));
    __asm__ volatile ("mv a0, %0" : : "r" (CODE_SEM_OPEN));
    __asm__ volatile ("ecall");

    int status;
    __asm__ volatile ("mv %0, a0" : "=r" (status));
    return status;
}

int sem_close(sem_t handle) {
    __asm__ volatile ("mv a1, %0" : : "r" (handle));
    __asm__ volatile ("mv a0, %0" : : "r" (CODE_SEM_CLOSE));
    __asm__ volatile ("ecall");

    int status;
    __asm__ volatile ("mv %0, a0" : "=r" (status));
    return status;
}

int sem_wait(sem_t id) {
    __asm__ volatile ("mv a1, %0" : : "r" (id));
    __asm__ volatile ("mv a0, %0" : : "r" (CODE_SEM_WAIT));
    __asm__ volatile ("ecall");

    int status;
    __asm__ volatile ("mv %0, a0" : "=r" (status));
    return status;
}
int sem_signal(sem_t id) {
    __asm__ volatile ("mv a1, %0" : : "r" (id));
    __asm__ volatile ("mv a0, %0" : : "r" (CODE_SEM_SIGNAL));
    __asm__ volatile ("ecall");

    int status;
    __asm__ volatile ("mv %0, a0" : "=r" (status));
    return status;
}

int sem_timedwait(sem_t id, time_t timeout) {
    __asm__ volatile ("mv a2, %0" : : "r" (timeout));
    __asm__ volatile ("mv a1, %0" : : "r" (id));
    __asm__ volatile ("mv a0, %0" : : "r" (CODE_SEM_TIMEDWAIT));
    __asm__ volatile ("ecall");

    int status;
    __asm__ volatile ("mv %0, a0" : "=r" (status));
    return status;
}
int sem_trywait(sem_t id) {
    __asm__ volatile ("mv a1, %0" : : "r" (id));
    __asm__ volatile ("mv a0, %0" : : "r" (CODE_SEM_TRYWAIT));
    __asm__ volatile ("ecall");

    int status;
    __asm__ volatile ("mv %0, a0" : "=r" (status));
    return status;
}

int time_sleep (time_t t) {
    __asm__ volatile ("mv a1, %0" : : "r" (t));
    __asm__ volatile ("mv a0, %0" : : "r" (CODE_TIME_SLEEP));
    __asm__ volatile ("ecall");

    int status;
    __asm__ volatile ("mv %0, a0" : "=r" (status));
    return status;
}
char getc() {
    __asm__ volatile ("mv a0, %0" : : "r" (CODE_GETC));
    __asm__ volatile ("ecall");

    char c;
    __asm__ volatile ("mv %0, a0" : "=r" (c));
    return c;
}

void putc(char c) {
    __asm__ volatile ("mv a1, %0" : : "r" (c));
    __asm__ volatile ("mv a0, %0" : : "r" (CODE_PUTC));
    __asm__ volatile ("ecall");
}
