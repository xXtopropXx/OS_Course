//
// Created by ofir on 20/03/16.
//

#ifndef OS_THREAD_H
#define OS_THREAD_H

#include <setjmp.h>
#include "uthreads.h"

#define NOT_CREATED_ID -1
/**
 * Enum for thread states
 */
enum State { Running, Sleeping, Ready, Blocked };

class Thread
{

public:
    /* Default C-tor for Thread */
    inline Thread():Thread(NOT_CREATED_ID){}
    /* ID C-tor for Thread */
    inline Thread(int id):Thread(id, nullptr){}
    /* ID and entry point C-tor for Thread */
    inline Thread(int id, void (*f)(void)){this->id = id;
        this->func = f; state = Ready; quantumsUsed = 0;}
    /* Checks whether the thread is an empty thread */
    inline bool isVoid() const {return id == NOT_CREATED_ID;}
    /* ID getter */
    inline int getID() const {return this->id;}
    /* State getter */
    inline const State getState() const {return this->state;}
    /* Gets the number of quantums the thread allready ran */
    inline int getQuantamsUsed() const {return this->quantumsUsed;}
    /* Gets the number of quantums the thread is left before waking up, 0 if
     * isn't at sleep mode
     */
    inline int getQuantimsTillWakeUp() const {return this->quantumsTillWakeUp;}
    /* Checks whether the thread should be awake*/
    inline bool shouldWake() const {return this->quantumsTillWakeUp <= 0;}
    /* reduces number of quantums till waking up */
    inline void reduceQuantumsTillWakeUp(){this->quantumsTillWakeUp--;}
    /* Terminates the thread */
    inline void terminate();
    /* Blocks the thread */
    inline void block() {this->state = Blocked;}
    /* Set the thread state as ready */
    inline void setAsReady(){this->state = Ready;}
    /* Makes the thread sleep for input number of quantums */
    inline void sleep(int q){this->state = Sleeping;
        this->quantumsTillWakeUp = q;}
    /* Makes the thread the running thread */
    inline void run(){this->state = Running; quantumsUsed++;}
    Thread& operator=(const Thread& other);

private:

    int id;
    State state;
    char stack[STACK_SIZE] = {'\0'};
    void (*func)(void);
    int quantumsUsed;
    int quantumsTillWakeUp;


};

/**
 * operator == overload between two threads.
 */
inline bool operator==(const Thread& lhs, const Thread& rhs) {
    return rhs.getID() == lhs.getID();
}

/** Terminates the thread */
inline void Thread::terminate() {
    this->id = NOT_CREATED_ID;
    this->state = Ready;
    for(int i = 0; i < STACK_SIZE; i++)
        this->stack[i] = '\0';
    this->func = nullptr;

}

Thread& Thread::operator=(const Thread& other)
{
    //TODO implement
    return *this;
}
#endif //OS_THREAD_H
