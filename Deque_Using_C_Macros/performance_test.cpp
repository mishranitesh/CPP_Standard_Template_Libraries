/*
 * Compile with -ldl.
 */

#include <dlfcn.h>
#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <cstdint>
#include <random>
#include <unistd.h>
#include "deque.hpp"
#include <chrono>

bool int_less(const int &o1, const int &o2) {
    return o1 < o2;
}

Deque_Custom(int)

int main() {

    FILE *devnull = fopen("/dev/null", "w");
    assert(devnull != 0);

    using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
    using Milli = std::chrono::duration<double, std::ratio<1, 1000>>;
    using namespace std::chrono;

    TimePoint start, end;
    start = system_clock::now();

    // Test random access performance
    {
        size_t sum = 0;
        int lo = 0, hi = 10000000;
        Deque_int deq;
        Deque_int_ctor(&deq, int_less);

        for (int i = lo; i < hi; i++) {
            deq.push_back(&deq, i);
        }

        for (int i = lo; i < hi; i++) {
            sum += deq.at(&deq, i);
        }

        deq.dtor(&deq);

        assert(sum == 49999995000000);
    }

    end = system_clock::now();
    Milli elapsed = end - start;
    assert(elapsed.count() < 2000);

    int rv = fclose(devnull);
    assert(rv == 0);
}
