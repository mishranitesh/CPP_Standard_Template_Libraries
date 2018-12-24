CFLAGS = -Wall -Wextra -pedantic -g

all: functionality_test.o
	g++ $(CFLAGS) $^ -o functionality_exec

functionality_test.o: functionality_test.cpp function.hpp
	g++ -c $(CFLAGS) functionality_test.cpp -o $@

run:
	valgrind ./functionality_exec

clean:
	rm -rf functionality_test.o functionality_exec
