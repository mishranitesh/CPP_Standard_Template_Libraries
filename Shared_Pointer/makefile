CFLAGS = -Wall -Wextra -pedantic -g -pthread

all: sharedptr.o
	g++ $(CFLAGS) $^ -o sharedptr_exec

sharedptr.o: functionality_test.cpp sharedptr.hpp
	g++ $(CFLAGS) -c functionality_test.cpp -o $@

run:
	./sharedptr_exec

clean:
	rm -rf sharedptr_exec sharedptr.o
