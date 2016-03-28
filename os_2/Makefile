CPPFLAGS  =-std=c++11 -Wall -Wextra -g

all: libuthreads.a

libuthreads.o: uthreads.cpp uthreads.h
	g++ $(CPPFLAGS) -c uthreads.cpp
	
libuthreads.a: libuthreads.o
	ar rcs libuthreads.a uthreads.o
	
clean:
	rm -f *.o *.a libuthreads
