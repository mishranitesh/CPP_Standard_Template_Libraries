CFLAGS = -Wall -Wextra -pedantic -g

all: rvalue_test.o
	g++ $(CFLAGS) $^ -o rvalue_test_exec

rvalue_test.o: functionality_test.cpp rvalue.hpp
	g++ -c $(CFLAGS) functionality_test.cpp -o $@

run:
	valgrind ./rvalue_test_exec

clean:
	rm -rf rvalue_test.o rvalue_test_exec
