//
// Created by ofir on 21/03/16.
//

#ifndef OS_QUEUEFACADE_H
#define OS_QUEUEFACADE_H
#include <vector>
using namespace std;
/**
 * Facade class to ease use of std queue with vector
 */
class myQueue{

public:
    ~myQueue(){} /** De-tor for the myQueue Facade class */
    void push(int n) {_q.push_back(n);} // good, old, normal push
    int pop(){int value = _q[0]; _q.erase(_q.begin()); return value;} // Pop
    int find(int value); // Finds the pos of the value in the que, -1 if not found
    void erase(int pos){_q.erase(_q.begin() + pos);}// deletes a value from the queue
private:
    vector<int> _q;
};

/**
 * Finds the position in the queue of the value it gets.
 * returns -1 if not found and the position otherwise.
 */
int myQueue::find(int value) {
    for(int i = 0; i < _q.size(); i++)
        if(_q[i] == value)
            return i;
    return -1;
}

#endif //OS_QUEUEFACADE_H
