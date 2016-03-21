#include "uthreads.h"
#include "threads_library.h"



using namespace std;


static ThreadsLibrary tl;

/*
 * Description: This function initializes the thread library.
 * You may assume that this function is called before any other thread library
 * function, and that it is called exactly once. The input to the function is
 * the length of a quantumDuration in micro-seconds. It is an error to call this
 * function with non-positive quantum_usecs.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_init(int quantum_usecs)
{
    tl = ThreadsLibrary(quantum_usecs);
    Thread main = Thread(MAIN_THREAD_ID);
    tl.addThread(main);
    tl.updateRunning();
    tl.updateRunning();
}

/*
 * Description: This function creates a new thread, whose entry point is the
 * function f with the signature void f(void). The thread is added to the end
 * of the READY threads list. The uthread_spawn function should fail if it
 * would cause the number of concurrent threads to exceed the limit
 * (MAX_THREAD_NUM). Each thread should be allocated with a stack of size
 * STACK_SIZE bytes.
 * Return value: On success, return the ID of the created thread.
 * On failure, return -1.
*/
int uthread_spawn(void (*f)(void))
{
    int tID;
    try {
        tID  = tl.getNextID();
    } catch(const threadsLimitException& e){
        cerr << LIB_FUNCTION_ERROR_PROLOG <<  e.what() << endl;
        return FAIL;
    }
    tl.addThread(Thread(tID, f));
    return tID;
}


/*
 * Description: This function terminates the thread with ID tid and deletes
 * it from all relevant control structures. All the resources allocated by
 * the library for this thread should be released. If no thread with ID tid
 * exists it is considered as an error. Terminating the main thread
 * (tid == 0) will result in the termination of the entire process using
 * exit(0) [after releasing the assigned library memory].
 * Return value: The function returns 0 if the thread was successfully
 * terminated and -1 otherwise. If a thread terminates itself or the main
 * thread is terminated, the function does not return.
*/
int uthread_terminate(int tid)
{
    Thread toTerminate;
    try {
        toTerminate = tl.getThreadAt(tid);
    } catch(const exception &e) {
        cerr << LIB_FUNCTION_ERROR_PROLOG <<  e.what() << endl;
        return FAIL;
    }
    if(tid == MAIN_THREAD_ID)
        exit(EXIT_SUCCESS);
    myQueue list;
    switch(toTerminate.getState()) {
        case Running :
            // TODO add terminating self
            tl.updateRunning();
        case Ready :
            list = tl.getReadyList();
            break;
        case Sleeping :
            list = tl.getSleepingList();
            break;
        case Blocked:
            list = tl.getBlockedList();
            break;
        default:
            break;
    }
    list.erase(list.find(tid));
    toTerminate.terminate();
    return SUCCESS;

}


/*
 * Description: This function blocks the thread with ID tid. The thread may
 * be resumed later using uthread_resume. If no thread with ID tid exists it
 * is considered as an error. In addition, it is an error to try blocking the
 * main thread (tid == 0). If a thread blocks itself, a scheduling decision
 * should be made. Blocking a thread in BLOCKED or SLEEPING states has no
 * effect and is not considered as an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_block(int tid)
{
    Thread toBlock;
    try {
        toBlock = tl.getThreadAt(tid);
    } catch(const exception &e) {
            cerr << LIB_FUNCTION_ERROR_PROLOG <<  e.what() << endl;
            return FAIL;
    }

    if(tid == MAIN_THREAD_ID) {
        cerr << LIB_FUNCTION_ERROR_PROLOG <<  BLOCK_MAIN_ERROR << endl;
        return FAIL;
    }
    myQueue list;
    switch(toBlock.getState()) {
        case Running :
            // TODO add blocking self
            tl.updateRunning();// update running
        case Ready :
            list = tl.getReadyList();
            break;
        default:
            return SUCCESS;
    }
    list.erase(list.find(tid));
    tl.getBlockedList().push(toBlock.getID());
    toBlock.block();
    //tl.run()
    return SUCCESS;
}


/*
 * Description: This function resumes a blocked thread with ID tid and moves
 * it to the READY state. Resuming a thread in the RUNNING, READY or SLEEPING
 * state has no effect and is not considered as an error. If no thread with
 * ID tid exists it is considered as an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_resume(int tid)
{
    Thread toResume;
    try {
        toResume = tl.getThreadAt(tid);
    } catch(const exception &e) {
        cerr << LIB_FUNCTION_ERROR_PROLOG <<  e.what() << endl;
        return FAIL;
    }
    myQueue list;
    switch(toResume.getState()) {
        case Blocked :
            list = tl.getBlockedList();
            break;
        default:
            return SUCCESS;
    }
    list.erase(list.find(tid));
    tl.getReadyList().push(toResume.getID());
    toResume.setAsReady();
    return SUCCESS;
}


/*
 * Description: This function puts the RUNNING thread to sleep for a period
 * of num_quantums (not including the current quantumDuration) after which it is moved
 * to the READY state. num_quantums must be a positive number. It is an error
 * to try to put the main thread (tid==0) to sleep. Immediately after a thread
 * transitions to the SLEEPING state a scheduling decision should be made.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_sleep(int num_quantums)
{
    if(num_quantums <= 0 ||
       tl.getRunningThread()->getID() == MAIN_THREAD_ID) {
        cerr << LIB_FUNCTION_ERROR_PROLOG << SLEEP_MAIN_ERROR << endl;
        return FAIL; // TODO seperate ifs
    }
    Thread* toSleep = tl.getRunningThread();
    toSleep->sleep(num_quantums);
    tl.setRunningThread(tl.getThreadAt(tl.getReadyList().pop())); //TODO change
}


/*
 * Description: This function returns the number of quantums until the thread
 * with id tid wakes up including the current quantumDuration. If no thread with ID
 * tid exists it is considered as an error. If the thread is not sleeping,
 * the function should return 0.
 * Return value: Number of quantums (including current quantumDuration) until wakeup.
*/
int uthread_get_time_until_wakeup(int tid)
{
    try {
        return tl.getThreadAt(tid).getQuantimsTillWakeUp();
        // TODO need to make sure contains this quantumDuration too
    } catch(exception& e) {
        cerr << LIB_FUNCTION_ERROR_PROLOG << e.what() << endl;
        return FAIL;
    }

}


/*
 * Description: This function returns the thread ID of the calling thread.
 * Return value: The ID of the calling thread.
*/
int uthread_get_tid()
{
    return tl.getRunningThread()->getID();
}


/*
 * Description: This function returns the total number of quantums that were
 * started since the library was initialized, including the current quantumDuration.
 * Right after the call to uthread_init, the value should be 1.
 * Each time a new quantumDuration starts, regardless of the reason, this number
 * should be increased by 1.
 * Return value: The total number of quantums.
*/
int uthread_get_total_quantums()
{
    tl.getTotalQuantums();
}


/*
 * Description: This function returns the number of quantums the thread with
 * ID tid was in RUNNING state. On the first time a thread runs, the function
 * should return 1. Every additional quantumDuration that the thread starts should
 * increase this value by 1 (so if the thread with ID tid is in RUNNING state
 * when this function is called, include also the current quantumDuration). If no
 * thread with ID tid exists it is considered as an error.
 * Return value: On success, return the number of quantums of the thread with ID tid. On failure, return -1.
*/
int uthread_get_quantums(int tid)
{
    Thread toTell;
    try {
        toTell = tl.getThreadAt(tid);
    } catch(const exception &e) {
        cerr << LIB_FUNCTION_ERROR_PROLOG <<  e.what() << endl;
        return FAIL;
    }
    return toTell.getQuantamsUsed();

}