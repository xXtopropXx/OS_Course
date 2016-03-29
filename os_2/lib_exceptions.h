
#ifndef OS_LIB_EXCEPTIONS_H
#define OS_LIB_EXCEPTIONS_H
#include <exception>

#define EXCEEDED_NUMBER_OF_THREADS_LOG "Exceeded number of valid threads"
#define VOID_THREAD_OPERATION_LOG "Tried to operate on a void thread"
#define SIG_ACTION_ERROR "Error in handling signals"
#define SET_ITIMER_ERROR "Setting time failed"
#define DOUBLE_INIT_ERROR "Process already initialize"

using namespace std;
/**
 * Class for exceeding number of allowed threads error
 */
class threadsLimitException: public exception {
public:
    const char* what() const noexcept { return EXCEEDED_NUMBER_OF_THREADS_LOG;}
};

/**
 * Class for trying to execute on an empty thread error.
 */
class noSuchThreadException: public exception {
public:
    const char* what() const noexcept{ return VOID_THREAD_OPERATION_LOG;}
};

/**
 * Class for signal handling error.
 */
class sigActionException: public exception {
public:
    const char* what() const noexcept{ return SIG_ACTION_ERROR;}
};

/**
 * Class for set time error.
 */
class setItimerException: public exception {
public:
    const char* what() const noexcept{ return SET_ITIMER_ERROR;}
};


/**
 * Class for set time error.
 */
class doubleInitException: public exception {
public:
    const char* what() const noexcept{ return DOUBLE_INIT_ERROR;}
};

#endif //OS_LIB_EXCEPTIONS_H
