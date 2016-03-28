//
// Created by ofir on 27/03/16.
//

#include "uthreads.h"
#include <iostream>
#include <unistd.h>
#define MICRO_IN_SEC 1000000
using namespace std;
void printOnce(string);
void f(void);
int main() {

    uthread_init(500000);
    cout << "finished uthread_init()" << endl;
    int id = uthread_spawn(f);
    cout << "f in " << id << endl;
    for(;;){printOnce("main");}
    return 0;
}

void f(){
    for(;;){printOnce("f");}
}

void printOnce(string s) {
    static string a = "";
    if(a == "") {
        a = s;
        cout << s << endl;
    }
    if(a == s)
        return;
    else
        cout << s << endl;
    a = s;
    return;

}

