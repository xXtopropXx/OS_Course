//
// Created by ofir on 21/03/16.
//

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

using namespace std;
extern void catchTimer(int sig);
class ThreadsLibrary
{
public:
    inline ~ThreadsLibrary();
    inline ThreadsLibrary(): quantumDuration(-1){} // Default C-tor
    inline ThreadsLibrary(int quantum_time);
    inline int getQuantumTime() const{return this->quantumDuration;}
    inline myQueue& getReadyList() {return this->ready;}
    inline myQueue& getSleepingList() {return this->sleeping;}
    inline myQueue& getBlockedList() {return this->blocked;}
    inline Thread* getRunningThread() const {return this->running;}
    void updateSleeping();
    inline void addThread(Thread t);
    inline int getNextID() const;
    inline Thread* getThreadAt(int tid);
    inline int getTotalQuantums(){return this->totalQuantumsUsed;}
    void setRunningThread(Thread* thread);
    void useQuantum(){totalQuantumsUsed++;}
    inline struct itimerval* getTimer(){return &timer;}
    const sigset_t *getBlockedSignals(){return &blockedSignals;}
    Thread* (threads[MAX_THREAD_NUM]) = {};

private:
    int totalQuantumsUsed;
    int quantumDuration;
    myQueue ready;
    myQueue sleeping;
    myQueue blocked;
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
 * gets thread to add to the threads list, appends it to the ready threads
 * queue.
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
inline Thread* ThreadsLibrary::getThreadAt(int tid)
{
    if (tid < MIN_ID || tid >= MAX_THREAD_NUM)
        throw out_of_range("Input is out of bound");
    else if(threads[tid] == nullptr) {
        throw noSuchThreadException();
    }
    else
        return threads[tid];
}


void ThreadsLibrary::setRunningThread(Thread* thread) {
    running = thread;
}



void ThreadsLibrary::updateSleeping() {
    myQueue toErase;
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

inline ThreadsLibrary::ThreadsLibrary(int quantum_time) {
    quantumDuration = quantum_time;
    timer.it_value.tv_sec = quantum_time / MICROSEC_IN_SEC;
    timer.it_value.tv_usec = quantum_time % MICROSEC_IN_SEC;
    timer.it_interval.tv_sec = quantum_time / MICROSEC_IN_SEC;
    timer.it_interval.tv_usec = quantum_time % MICROSEC_IN_SEC;

    sigemptyset(&blockedSignals);
    sigaddset(&blockedSignals, SIGVTALRM);
}

inline ThreadsLibrary::~ThreadsLibrary() {
    for(int i = 0; i < MAX_THREAD_NUM; i++)
        if(threads[i] != nullptr)
            delete threads[i];
}

#endif //OS_THREADS_LIBRARY_H_