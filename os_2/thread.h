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
    inline Thread(int id):id(id),quantumsUsed(0),quantumsTillWakeUp(0){}
    /* ID getter */
    inline int getID() const {return id;}
    /* Gets the number of quantums the thread allready ran */
    inline int getQuantamsUsed() {return quantumsUsed;}
    /* Gets the number of quantums the thread is left before waking up, 0 if
     * isn't at sleep mode
     */
    inline int getQuantimsTillWakeUp() const {return quantumsTillWakeUp;}
    /* Checks whether the thread should be awake*/
    inline bool shouldWake() { return quantumsTillWakeUp <= 0;}
    /* reduces number of quantums till waking up */
    inline void reduceQuantumsTillWakeUp(){quantumsTillWakeUp--;}
    /* Makes the thread sleep for input number of quantums */
    inline void sleep(int q){quantumsTillWakeUp = q;}
    inline void useQuantum() {quantumsUsed++;}
    sigjmp_buf env;
    char stack[STACK_SIZE] = {'\0'};
private:
    int id;
    int quantumsUsed;
    int quantumsTillWakeUp;
};
#endif //OS_THREAD_H


