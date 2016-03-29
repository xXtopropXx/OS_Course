
#ifndef OS_THREADS_LIBRARY_H
#define OS_THREADS_LIBRARY_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include <signal.h>
#include <sys/time.h>

#include "thread.h"
#include "lib_exceptions.h"
#include "queueFacade.h"

#define SYS_CALL_ERROR_PROLOG "system error: "
#define LIB_FUNCTION_ERROR_PROLOG "thread library error: "
#define BLOCK_MAIN_ERROR "Tried to block main thread. (tid == 0)"
#define SLEEP_MAIN_ERROR "Tried to make main thread sleep. (tid == 0)"
#define SLEEP_INVALID_ARGUMENT \
"Tried to make thread sleep with non-positive number of quantums."

#define SUCCESS 0
#define FAIL -1

#define MAIN_THREAD_ID 0
#define MIN_ID 0

#define MICROSEC_IN_SEC 1000000

#define SAVE_SIGS 1
#define SIGSETJMP_RETURNING_DIRECTLY 0
#define SIGLONGJMP_RET_VAL 1

using namespace std;
extern void catchTimer(int sig);
class ThreadsLibrary
{
public:
    inline ~ThreadsLibrary(); //D-tor
    inline ThreadsLibrary(): quantumDuration(-1){} // Default C-tor
    inline ThreadsLibrary(int quantum_time); // C-tor
    /* returns the ready threads list */
    inline MyQueue& getReadyList() {return this->ready;}
    /* returns the sleeping threads list */
    inline MyQueue& getSleepingList() {return this->sleeping;}
    /* returns the blocked threads list */
    inline MyQueue& getBlockedList() {return this->blocked;}
    /* returns the current running thread */
    inline Thread* getRunningThread() const {return this->running;}
    /* returns the total quantums used so far */
    inline int getTotalQuantums(){return this->totalQuantumsUsed;}
    /* return the lib timer */
    inline struct itimerval* getTimer(){return &timer;}
    /* return the block signals */
    inline const sigset_t *getBlockedSignals(){return &blockedSignals;}
    /* add one to the total quantum counter */
    inline void useQuantum(){totalQuantumsUsed++;}

    void updateSleeping();
    inline void addThread(Thread t);
    inline int getNextID() const;
    inline Thread* getThreadAt(int tid);
    void setRunningThread(Thread* thread);
    Thread* (threads[MAX_THREAD_NUM]) = {};

private:
    int totalQuantumsUsed;
    int quantumDuration;
    MyQueue ready;
    MyQueue sleeping;
    MyQueue blocked;
    Thread* running;

    struct itimerval timer;
    sigset_t blockedSignals;
};

/**
 * Finds the next available id
 */
inline int ThreadsLibrary::getNextID() const {
    for(int i = 0; i < MAX_THREAD_NUM; i++) {
        if(threads[i] == nullptr)
            return i;
    }
    throw threadsLimitException();
}

/**
 * gets thread to add to the threads list,
 * appends it to the ready threads queue.
 */
inline void ThreadsLibrary::addThread(Thread t) {
    threads[t.getID()] = new Thread(t);
    if(running == nullptr) {
        running = threads[t.getID()];
        running->useQuantum();
    }
    else {
        ready.push(t.getID());
    }

}

/**
 * Gets the Thread with the respective id.
 */
inline Thread* ThreadsLibrary::getThreadAt(int tid) {
    if (tid < MIN_ID || tid >= MAX_THREAD_NUM)
        throw out_of_range("Input is out of bound");
    else if(threads[tid] == nullptr) {
        throw noSuchThreadException();
    }
    else
        return threads[tid];
}

/**
 * change the current running thread to be the given thread
 */
void ThreadsLibrary::setRunningThread(Thread* thread) {
    running = thread;
}

/**
 * take care of all the sleeping threads.
 * decrease the number of quantums till wake up,
 * and check if there is threads need to be awaken.
 */
void ThreadsLibrary::updateSleeping() {
    MyQueue toErase;
    for(auto it = sleeping.begin(); it != sleeping.end(); it++) {
        Thread* t = getThreadAt(*it);
        t->reduceQuantumsTillWakeUp();
        if(t->shouldWake()) {
            toErase.push(t->getID());
        }
    }
    while(toErase.size()) {
        int id = toErase.pop();
        ready.push(id);
        sleeping.erase(sleeping.find(id));
    }
}

/**
 * the C-tor
 */
inline ThreadsLibrary::ThreadsLibrary(int quantum_time) {
    quantumDuration = quantum_time;
    timer.it_value.tv_sec = quantum_time / MICROSEC_IN_SEC;
    timer.it_value.tv_usec = quantum_time % MICROSEC_IN_SEC;
    timer.it_interval.tv_sec = quantum_time / MICROSEC_IN_SEC;
    timer.it_interval.tv_usec = quantum_time % MICROSEC_IN_SEC;

    sigemptyset(&blockedSignals);
    sigaddset(&blockedSignals, SIGVTALRM);
}

/**
 * D-tor
 */
inline ThreadsLibrary::~ThreadsLibrary() {
    for(int i = 0; i < MAX_THREAD_NUM; i++)
        if(threads[i] != nullptr)
            delete threads[i];
}

#endif //OS_THREADS_LIBRARY_H_
