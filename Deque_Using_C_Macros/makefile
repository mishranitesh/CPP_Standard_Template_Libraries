CFLAGS = -g -O4 -Wall -Wextra -pedantic

test:
	g++ $(CFLAGS) -ldl functionality_test.cpp -o test_exec
	./test_exec
	rm -rf test_exec

test_checkmem:
	g++ $(CFLAGS) -ldl functionality_test.cpp -o test_exec
	valgrind --leak-check=summary ./test_exec
	rm -rf test_exec

perf:
	g++ $(CFLAGS) -ldl performance_test.cpp -o perf_exec
	./perf_exec
	rm -rf perf_exec
