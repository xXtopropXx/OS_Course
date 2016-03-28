//
// Created by ofir on 20/03/16.
//

#ifndef OS_THREAD_H
#define OS_THREAD_H

#include <setjmp.h>
#include "uthreads.h"
#include <unistd.h>


#ifdef __x86_64__
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
            "rol    $0x11,%0\n"
    : "=g" (ret)
    : "0" (addr));
    return ret;
}


#else
/* code for 32 bit Intel arch */

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%gs:0x18,%0\n"
		"rol    $0x9,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}
#endif

#define NOT_CREATED_ID -1


class Thread
{

public:
    /* Default C-tor for Thread */
    inline Thread():Thread(NOT_CREATED_ID){ std:: cout << "C-tor" << std::endl;}
    /* ID C-tor for Thread */
    inline Thread(int id):Thread(id, nullptr){}
    /* ID and entry point C-tor for Thread */
    inline Thread(int id, void (*f)(void));
    /* Checks whether the thread is an empty thread */
    inline bool isVoid() const {return id == NOT_CREATED_ID;}
    /* ID getter */
    inline int getID() const {return id;}
    /* Gets the number of quantums the thread allready ran */
    inline int getQuantamsUsed() const {return quantumsUsed;}
    /* Gets the number of quantums the thread is left before waking up, 0 if
     * isn't at sleep mode
     */
    inline int getQuantimsTillWakeUp() const {return quantumsTillWakeUp;}
    /* Checks whether the thread should be awake*/
    inline bool shouldWake() const{return !isVoid() && quantumsTillWakeUp <= 0;}
    /* reduces number of quantums till waking up */
    inline void reduceQuantumsTillWakeUp(){if(!isVoid()){quantumsTillWakeUp--;}}
    /* Terminates the thread */
    inline void terminate();
    /* Makes the thread sleep for input number of quantums */
    inline void sleep(int q){quantumsTillWakeUp = q;}
    Thread& operator=(const Thread& other);
    inline void useQuantum() {quantumsUsed++;}
    sigjmp_buf env;

private:

    int id;
    char stack[STACK_SIZE] = {'\0'};

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
    id = NOT_CREATED_ID;
    for(int i = 0; i < STACK_SIZE; i++)
        stack[i] = '\0';
    env->__jmpbuf[JB_SP] = translate_address((address_t) 0);
    env->__jmpbuf[JB_PC] = translate_address((address_t) 0);
    quantumsUsed = 0;
    quantumsTillWakeUp = 0;

}

/**
 * Assignment operator for Thread class
 */
Thread& Thread::operator=(const Thread& other) {
    id = other.id;
    env->__jmpbuf[JB_PC] = other.env->__jmpbuf[JB_PC];
    env->__jmpbuf[JB_SP] = other.env->__jmpbuf[JB_SP];
    quantumsUsed = other.quantumsUsed;
    quantumsTillWakeUp = other.quantumsTillWakeUp;
    return *this;
}

Thread::Thread(int id, void (*f)()) {
    this->id = id;

    sigsetjmp(env, 1);
    env->__jmpbuf[JB_SP] = translate_address(
            (address_t) stack + STACK_SIZE - sizeof(address_t));
    env->__jmpbuf[JB_PC] = translate_address((address_t)f);
    sigemptyset(&(env->__saved_mask));
    quantumsUsed = 0;
    quantumsTillWakeUp = 0;
}
#endif //OS_THREAD_H


