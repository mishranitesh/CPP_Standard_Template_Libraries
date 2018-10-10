#include <dlfcn.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <random>
#include <unistd.h>
#include "deque.hpp"

/*
 * Test for User Defined class
 */
struct MyClass {
    int id;
    char name[10];
};

bool MyClass_less_by_id(const MyClass &o1, const MyClass &o2) {
    return o1.id < o2.id;
}

void MyClass_print(const MyClass *o) {
    printf("%d\n", o->id);
    printf("%s\n", o->name);
}
Deque_Custom(MyClass)

/*
 * Test for Primitive Data Type (int)
 */
bool int_less(const int &o1, const int &o2) {
    return o1 < o2;
}
Deque_Custom(int)

int main() {
    FILE *devnull = fopen("/dev/null", "w");
    assert(devnull != 0);

    {
        Deque_MyClass deq;
        Deque_MyClass_ctor(&deq, MyClass_less_by_id);

        assert(deq.size(&deq) == 0);
        assert(deq.empty(&deq));

        // Print "---- Deque_MyClass, 14".
        printf("---- %s, %d\n", deq.type_name, (int) sizeof(deq.type_name));
        assert(sizeof deq.type_name == 14);

        deq.push_back(&deq, MyClass{1, "Joe"});
        deq.push_back(&deq, MyClass{2, "Mary"});
        deq.push_back(&deq, MyClass{3, "Tom"});
        deq.push_front(&deq, MyClass{0, "Mike"});
        deq.push_front(&deq, MyClass{-1, "Mary"});

        MyClass_print(&deq.front(&deq));
        MyClass_print(&deq.back(&deq));
        assert(deq.front(&deq).id == -1);
        assert(deq.back(&deq).id == 3);

        deq.pop_front(&deq);
        deq.pop_back(&deq);
        assert(deq.front(&deq).id == 0);
        assert(deq.back(&deq).id == 2);

        assert(deq.size(&deq) == 3);

        for (Deque_MyClass_Iterator it = deq.begin(&deq);
             !Deque_MyClass_Iterator_equal(it, deq.end(&deq)); it.inc(&it)) {
            MyClass_print(&it.deref(&it));
        }

        // Multiple iterators
        for (Deque_MyClass_Iterator it1 = deq.begin(&deq);
             !Deque_MyClass_Iterator_equal(it1, deq.end(&deq)); it1.inc(&it1)) {
            MyClass_print(&it1.deref(&it1));
            for (Deque_MyClass_Iterator it2 = deq.begin(&deq);
                 !Deque_MyClass_Iterator_equal(it2, deq.end(&deq)); it2.inc(&it2)) {
                MyClass_print(&it2.deref(&it2));
                for (Deque_MyClass_Iterator it3 = deq.begin(&deq);
                     !Deque_MyClass_Iterator_equal(it3, deq.end(&deq)); it3.inc(&it3)) {
                    MyClass_print(&it3.deref(&it3));
                }
            }
        }

        // Test decrement of end.
        {
            auto it = deq.end(&deq);
            it.dec(&it);
            assert(it.deref(&it).id == 2);
        }

        for (size_t i = 0; i < 3; i++) {
            MyClass_print(&deq.at(&deq, i));
        }

        // Test that front(), back(), at(), and deref() are returning a reference.

        // Change via at().
        assert(deq.at(&deq, 0).id == 0);
        deq.at(&deq, 0).id = 100;
        assert(deq.at(&deq, 0).id == 100);

        // Change via front().
        assert(deq.front(&deq).id == 100);
        deq.front(&deq).id = 0;
        assert(deq.front(&deq).id == 0);
        assert(deq.at(&deq, 0).id == 0); // Verify with at() also.
        {
            auto it = deq.end(&deq);
            it.dec(&it);
            assert(it.deref(&it).id == 2);
            it.deref(&it).id = 200;
            assert(it.deref(&it).id == 200);

            // Change using back().
            assert(deq.back(&deq).id == 200);
            deq.back(&deq).id = 2;
            assert(deq.back(&deq).id == 2);
            assert(it.deref(&it).id == 2); // Verify with iterator also.
        }

        deq.clear(&deq);

        deq.dtor(&deq);

        /*
         * Test equality.
         * Two deques compare equal if they are of the same length and all the elements compare equal.
         * It is undefined behavior if the two deques were constructed with different comparison functions.
         */
        {
            Deque_MyClass deq1, deq2;
            Deque_MyClass_ctor(&deq1, MyClass_less_by_id);
            Deque_MyClass_ctor(&deq2, MyClass_less_by_id);

            deq1.push_back(&deq1, MyClass{1, "Joe"});
            deq1.push_back(&deq1, MyClass{2, "Jane"});
            deq1.push_back(&deq1, MyClass{3, "Mary"});
            deq2.push_back(&deq2, MyClass{1, "Joe"});
            deq2.push_back(&deq2, MyClass{2, "Jane"});
            deq2.push_back(&deq2, MyClass{3, "Mary"});

            assert(Deque_MyClass_equal(deq1, deq2));

            deq1.pop_back(&deq1);
            assert(!Deque_MyClass_equal(deq1, deq2));
            deq1.push_back(&deq1, MyClass{4, "Mary"});
            assert(!Deque_MyClass_equal(deq1, deq2));

            deq1.dtor(&deq1);
            deq2.dtor(&deq2);
        }
    }

    fclose(devnull);

    return 0;
}
