//
// Created by ofir on 21/03/16.
//

#ifndef OS_LIB_EXCEPTIONS_H
#define OS_LIB_EXCEPTIONS_H
#include <exception>
using namespace std;
/**
 * Class for exceeding number of allowed threads error
 */
class threadsLimitException: public exception {
public:
    const char* what() const noexcept { return "Exceeded number of valid threads";}
};

/**
 * Class for trying to execute on an empty thread error.
 */
class noSuchThreadException: public exception {
public:
    const char* what() const noexcept{ return "Tried to operate on a void thread";}
};

#endif //OS_LIB_EXCEPTIONS_H
