//
// Created by ofir on 21/03/16.
//

#ifndef OS_THREADS_LIBRARY_H
#define OS_THREADS_LIBRARY_H

#include <iostream>
#include <vector>
#include <stdexcept>

#include "thread.h"
#include "lib_exceptions.h"
#include "queueFacade.h"

#define LIB_FUNCTION_ERROR_PROLOG "thread library error: "
#define BLOCK_MAIN_ERROR "Tried to block main thread (tid == 0)"
#define SLEEP_MAIN_ERROR "Tried to sleep main thread (tid == 0)"
#define SUCCESS 0
#define FAIL -1

#define MAIN_THREAD_ID 0
#define MIN_ID 0

using namespace std;

class ThreadsLibrary
{
public:
    inline ThreadsLibrary(): quantumDuration(-1){} // Default C-tor
    inline ThreadsLibrary(int quantum_time): quantumDuration(quantum_time){}
    inline const int getQuantumTime() const {return this->quantumDuration;}
    inline myQueue getReadyList() const {return this->ready;}
    inline myQueue getSleepingList() const {return this->sleeping;}
    inline myQueue getBlockedList() const {return this->blocked;}
    inline Thread* getRunningThread() const {return this->running;}
    inline void addThread(Thread t);
    inline int getNextID() const;
    inline void updateRunning();
    inline Thread& getThreadAt(int tid);
    inline int getTotalQuantums(){return this->totalQuantumsUsed;}
    void setRunningThread(Thread &thread);

private:
    int totalQuantumsUsed;
    int quantumDuration;
    myQueue ready;
    myQueue sleeping;
    myQueue blocked;
    Thread* running;
    Thread threads[MAX_THREAD_NUM] = {};


};

/**
 * Finds the next available id
 */
inline int ThreadsLibrary::getNextID() const {
    for(int i = 0; i < MAX_THREAD_NUM; i++){
        if(this->threads[i].isVoid())
            return i;
    }
    throw threadsLimitException();
}

/**
 * gets thread to add to the threads list, appends it to the ready threads
 * queue.
 */
inline void ThreadsLibrary::addThread(Thread t) {
    this->threads[t.getID()] = t;
    this->ready.push(t.getID());
}

/**
 * Makes the thread with the tid parameter id as the running thread. Moves the
 * current running thread to the back of the ready threads queue
 */
inline void ThreadsLibrary::updateRunning() {
    if(this->running != nullptr) {
        this->ready.push(this->running->getID());
        this->running->setAsReady();
    }
    this->running = &this->getThreadAt(this->ready.pop());
    this->running->run();
}

/**
 * Gets the Thread with the respective id.
 */
inline Thread& ThreadsLibrary::getThreadAt(int tid)
{
    if (tid < MIN_ID || tid >= MAX_THREAD_NUM)
        throw out_of_range("Input is out of bound");
    else if(this->threads[tid].isVoid())
        throw noSuchThreadException();
    else
        return this->threads[tid];
}
#endif //OS_THREADS_LIBRARY_H


void ThreadsLibrary::setRunningThread(Thread &thread) {
    this->running = &thread;
}
