
#ifndef OS_QUEUEFACADE_H
#define OS_QUEUEFACADE_H
#include <vector>

#define NON_EXISTING_VALUE -1

using namespace std;
/**
 * Facade class to ease use of std queue with vector
 */
class MyQueue{

public:
    ~MyQueue(){} /** De-tor for the MyQueue Facade class */
    void push(int value) {_q.push_back(value);} // good, old, normal push
    int pop(){int value = _q[0]; _q.erase(_q.begin()); return value;} // Pop
    size_t size(){return _q.size();} //return myQueue current size
    /* Finds the pos of the value in the que, -1 if not found */
    int find(int value);
    /* deletes a value from the queue */
    vector<int>::iterator erase(int pos) {return _q.erase(_q.begin() + pos);}
    vector<int>::iterator begin(){return _q.begin();} //return begin iterator
    vector<int>::iterator end(){return _q.end();} //return end iterator
private:
    vector<int> _q; //my inside queue for the Facade
};

/**
 * Finds the position in the queue of the value it gets.
 * returns -1 if not found and the position otherwise.
 */
int MyQueue::find(int value) {
    for(unsigned int i = 0; i < _q.size(); i++)
        if(_q[i] == value)
            return i;
    return NON_EXISTING_VALUE;
}

#endif //OS_QUEUEFACADE_H
