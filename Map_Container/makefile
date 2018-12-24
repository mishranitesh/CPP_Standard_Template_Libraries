CFLAGS= -Wall -Wextra -pedantic -O4

all: map.hpp functionality_test.cpp
	g++ $(CFLAGS) functionality_test.cpp -o test_exec
	./test_exec
	rm -rf test_exec

checkmem: map.hpp functionality_test.cpp
	g++ $(CFLAGS) $^ -o test_exec
	valgrind ./test_exec
	rm -rf test_exec
