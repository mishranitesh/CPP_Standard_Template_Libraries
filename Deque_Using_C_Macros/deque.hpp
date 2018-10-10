#ifndef NITESH_DEQUE_H
#define NITESH_DEQUE_H

#include <cstring>
#include <algorithm>

#define LIST_CAPACITY 20
#define BEGIN_INDEX 0
#define DOUBLE 2

/*
 * Macro to implement methods related to Deque data structure using Stringification
 */
#define Deque_Custom(t)                                                                                                                         \
    using namespace std;                                                                                                                        \
    /* Forward declaration of structure */                                                                                                      \
    struct Deque_##t;                                                                                                                           \
    /* Structure to represent Iterator for Deque */                                                                                             \
    struct Deque_##t##_Iterator {                                                                                                               \
        /* Variable declaration for Deque Iterator */                                                                                           \
        struct Deque_##t *deque_##t;                                                                                                            \
        int deque_##t##_index;                                                                                                                  \
        /* Function Pointer declaration for Deque Iterator */                                                                                   \
        void (*inc)(Deque_##t##_Iterator *it);                                                                                                  \
        void (*dec)(Deque_##t##_Iterator *it);                                                                                                  \
        t &(*deref)(Deque_##t##_Iterator *it);                                                                                                  \
    };                                                                                                                                          \
    /* Structure to represent Deque */                                                                                                          \
    struct Deque_##t {                                                                                                                          \
        /* Variable declaration for Deque */                                                                                                    \
        char type_name[(sizeof "Deque_") + (sizeof #t) - 1];                                                                                    \
        t *t##_list;                                                                                                                            \
        int list_front_index;                                                                                                                   \
        int list_back_index;                                                                                                                    \
        size_t list_capacity;                                                                                                                   \
        size_t list_elements;                                                                                                                   \
        /* Function Pointer declaration for Deque */                                                                                            \
        size_t (*size)(Deque_##t *deq);                                                                                                         \
        bool (*empty)(Deque_##t *deq);                                                                                                          \
        void (*push_front)(Deque_##t *deq, t);                                                                                                  \
        void (*push_back)(Deque_##t *deq, t);                                                                                                   \
        void (*pop_front)(Deque_##t *deq);                                                                                                      \
        void (*pop_back)(Deque_##t *deq);                                                                                                       \
        t &(*front)(Deque_##t *deq);                                                                                                            \
        t &(*back)(Deque_##t *deq);                                                                                                             \
        t &(*at)(Deque_##t *deq, size_t i);                                                                                                     \
        Deque_##t##_Iterator (*begin)(Deque_##t *deq);                                                                                          \
        Deque_##t##_Iterator (*end)(Deque_##t *deq);                                                                                            \
        void (*clear)(Deque_##t *deq);                                                                                                          \
        void (*dtor)(Deque_##t *deq);                                                                                                           \
        bool (*compare_elements)(const t &o1, const t &o2);                                                                                     \
        void (*sort)(Deque_##t *deq, Deque_##t##_Iterator begin, Deque_##t##_Iterator end);                                                     \
    };                                                                                                                                          \
    /* Typedef for Deque and Deque Iterator structs */                                                                                          \
    typedef struct Deque_##t##_Iterator Deque_##t##_Iterator;                                                                                   \
    typedef struct Deque_##t Deque_##t;                                                                                                         \
    /* Function returns number of elements present in Deque List */                                                                             \
    size_t list_size(Deque_##t *deq) {                                                                                                          \
        return deq->list_elements;                                                                                                              \
    }                                                                                                                                           \
    /* Function returns true if Deque List is empty otherwise false */                                                                          \
    bool list_empty(Deque_##t *deq) {                                                                                                           \
        return (deq->list_elements == 0);                                                                                                       \
    }                                                                                                                                           \
    /* Function to double the capacity of Deque List when it is full with elements */                                                           \
    void dynamic_deq_capacity(Deque_##t *deq) {                                                                                                 \
        /* Allocating new memory with doubled size */                                                                                           \
        int front_index = deq->list_front_index, index = deq->list_front_index;                                                                 \
        size_t new_capacity = deq->list_capacity * DOUBLE;                                                                                      \
        auto *new_list = new t[new_capacity];                                                                                                   \
        if (new_list != nullptr) {                                                                                                              \
            memset(new_list, 0, new_capacity * sizeof(int));                                                                                    \
            /* Logic to copy old list data into the newly created list */                                                                       \
            while (deq->list_front_index != deq->list_back_index) {                                                                             \
                new_list[index] = deq->t##_list[deq->list_front_index];                                                                         \
                deq->list_front_index--;                                                                                                        \
                index--;                                                                                                                        \
                if (deq->list_front_index < 0) {                                                                                                \
                    deq->list_front_index += deq->list_capacity;                                                                                \
                }                                                                                                                               \
                if (index < 0) {                                                                                                                \
                    index += new_capacity;                                                                                                      \
                }                                                                                                                               \
            }                                                                                                                                   \
            /* Copying last element in new list and setting indexes to deque object */                                                          \
            new_list[index] = deq->t##_list[deq->list_front_index];                                                                             \
            auto *temp = deq->t##_list;                                                                                                         \
            deq->t##_list = nullptr;                                                                                                            \
            deq->t##_list = new_list;                                                                                                           \
            delete[] temp;                                                                                                                      \
            deq->list_front_index = front_index;                                                                                                \
            deq->list_back_index = index;                                                                                                       \
            deq->list_capacity = new_capacity;                                                                                                  \
        }                                                                                                                                       \
    }                                                                                                                                           \
    /* Function to push new element from front side to Deque List */                                                                            \
    void push_front_deque(Deque_##t *deq, t new_##t) {                                                                                          \
        int front_index = deq->list_front_index;                                                                                                \
        /* Logic to adjust front index to implement Dynamic Circular Array */                                                                   \
        if (!deq->empty(deq)) {                                                                                                                 \
            front_index = (front_index + 1) % deq->list_capacity;                                                                               \
        }                                                                                                                                       \
        deq->t##_list[front_index] = new_##t;                                                                                                   \
        deq->list_elements++;                                                                                                                   \
        deq->list_front_index = front_index;                                                                                                    \
        /* Increase the capacity of Deque once it is full */                                                                                    \
        if (deq->list_elements == deq->list_capacity) {                                                                                         \
            dynamic_deq_capacity(deq);                                                                                                          \
        }                                                                                                                                       \
    }                                                                                                                                           \
    /* Function to push new element from back side to Deque List */                                                                             \
    void push_back_deque(Deque_##t *deq, t new_##t) {                                                                                           \
        int back_index = deq->list_back_index;                                                                                                  \
        if (deq->empty(deq)) {                                                                                                                  \
            /* Element will be stored at the 0th index of myClass_list if it is empty */                                                        \
            deq->t##_list[back_index] = new_##t;                                                                                                \
            deq->list_elements++;                                                                                                               \
            deq->list_back_index = back_index;                                                                                                  \
        } else {                                                                                                                                \
            back_index--;                                                                                                                       \
            /* Handling condition if back_index becomes -1 (after successive two push_backs on empty myClass_list) */                           \
            if (back_index < 0) {                                                                                                               \
                back_index = back_index + deq->list_capacity;                                                                                   \
            }                                                                                                                                   \
            deq->t##_list[back_index] = new_##t;                                                                                                \
            deq->list_elements++;                                                                                                               \
            deq->list_back_index = back_index;                                                                                                  \
        }                                                                                                                                       \
        /* Increase the capacity of Deque once it is full */                                                                                    \
        if (deq->list_elements == deq->list_capacity) {                                                                                         \
            dynamic_deq_capacity(deq);                                                                                                          \
        }                                                                                                                                       \
    }                                                                                                                                           \
    /* Function to pop element from front side from Deque List */                                                                               \
    void pop_front_deque(Deque_##t *deq) {                                                                                                      \
        int front_index = deq->list_front_index;                                                                                                \
        if (!deq->empty(deq)) {                                                                                                                 \
            front_index--;                                                                                                                      \
            /* Adjust front index of list */                                                                                                    \
            if (front_index < 0) {                                                                                                              \
                front_index = front_index + deq->list_capacity;                                                                                 \
            }                                                                                                                                   \
            deq->list_elements--;                                                                                                               \
            deq->list_front_index = front_index;                                                                                                \
        }                                                                                                                                       \
    }                                                                                                                                           \
    /* Function to pop element from back side from Deque List */                                                                                \
    void pop_back_deque(Deque_##t *deq) {                                                                                                       \
        int back_index = deq->list_back_index;                                                                                                  \
        if (!deq->empty(deq)) {                                                                                                                 \
            /* Adjust back index of list */                                                                                                     \
            back_index = (back_index + 1) % deq->list_capacity;                                                                                 \
            deq->list_elements--;                                                                                                               \
            deq->list_back_index = back_index;                                                                                                  \
        }                                                                                                                                       \
    }                                                                                                                                           \
    /* Function returns element from front side from Deque List */                                                                              \
    t &front_deque(Deque_##t *deq) {                                                                                                            \
        return deq->t##_list[deq->list_front_index];                                                                                            \
    }                                                                                                                                           \
    /* Function returns element from back side from Deque List */                                                                               \
    t &back_deque(Deque_##t *deq) {                                                                                                             \
        return deq->t##_list[deq->list_back_index];                                                                                             \
    }                                                                                                                                           \
    /* Function returns element at the required index from Deque List */                                                                        \
    t &at_deque(Deque_##t *deq, size_t req_index) {                                                                                             \
        int index = deq->list_front_index - req_index;                                                                                          \
        if (index < 0) {                                                                                                                        \
            index = index + deq->list_capacity;                                                                                                 \
        }                                                                                                                                       \
        return deq->t##_list[index];                                                                                                            \
    }                                                                                                                                           \
    /* Function to increment the iterator index by 1 */                                                                                         \
    void inc_deque(Deque_##t##_Iterator *it) {                                                                                                  \
        it->deque_##t##_index++;                                                                                                                \
    }                                                                                                                                           \
    /* Function to decrement the iterator index by 1 */                                                                                         \
    void dec_deque(Deque_##t##_Iterator *it) {                                                                                                  \
        it->deque_##t##_index--;                                                                                                                \
    }                                                                                                                                           \
    /* Function returns the element at iterator index */                                                                                        \
    t &deref_deque(Deque_##t##_Iterator *it) {                                                                                                  \
        Deque_##t *deq_##t = it->deque_##t;                                                                                                     \
        int index = deq_##t->list_front_index - it->deque_##t##_index;                                                                          \
        if (index < 0) {                                                                                                                        \
            index = index + deq_##t->list_capacity;                                                                                             \
        }                                                                                                                                       \
        return deq_##t->t##_list[index];                                                                                                        \
    }                                                                                                                                           \
    /* Function returns Deque Iterator object with begin index as 0 */                                                                          \
    Deque_##t##_Iterator begin_deque(Deque_##t *deq) {                                                                                          \
        Deque_##t##_Iterator t##_iter{};                                                                                                        \
        t##_iter.deque_##t = deq;                                                                                                               \
        t##_iter.deque_##t##_index = BEGIN_INDEX;                                                                                               \
        t##_iter.dec = &dec_deque;                                                                                                              \
        t##_iter.deref = &deref_deque;                                                                                                          \
        t##_iter.inc = &inc_deque;                                                                                                              \
        return t##_iter;                                                                                                                        \
    }                                                                                                                                           \
    /* Function returns Deque Iterator object with end index as number of elements in Deque List */                                             \
    Deque_##t##_Iterator end_deque(Deque_##t *deq) {                                                                                            \
        Deque_##t##_Iterator t##_iter{};                                                                                                        \
        t##_iter.deque_##t = deq;                                                                                                               \
        t##_iter.deque_##t##_index = deq->list_elements;                                                                                        \
        t##_iter.dec = &dec_deque;                                                                                                              \
        t##_iter.deref = &deref_deque;                                                                                                          \
        t##_iter.inc = &inc_deque;                                                                                                              \
        return t##_iter;                                                                                                                        \
    }                                                                                                                                           \
    /* Function returns true when passed argument are equal otherwise false */                                                                  \
    bool Deque_##t##_Iterator_equal(Deque_##t##_Iterator start, Deque_##t##_Iterator end) {                                                     \
        return (start.deque_##t##_index == end.deque_##t##_index);                                                                              \
    }                                                                                                                                           \
    /* Function returns true when both Deque Lists are same otherwise false  */                                                                 \
    bool Deque_##t##_equal(Deque_##t deq1, Deque_##t deq2) {                                                                                    \
        /* Both parameters should have same number of elements */                                                                               \
        if (deq1.list_elements != deq2.list_elements) {                                                                                         \
            return false;                                                                                                                       \
        }                                                                                                                                       \
        /* Elements should be present at the same index in respective myClass_list */                                                           \
        for (size_t i = 0; i < deq1.list_elements; i++) {                                                                                       \
            t t##_1 = deq1.at(&deq1, i);                                                                                                        \
            t t##_2 = deq2.at(&deq2, i);                                                                                                        \
            if (deq1.compare_elements(t##_1, t##_2) || deq2.compare_elements(t##_2, t##_1)) {                                                   \
                return false;                                                                                                                   \
            }                                                                                                                                   \
        }                                                                                                                                       \
        return true;                                                                                                                            \
    }                                                                                                                                           \
    /* Function to reset all indexes as it was during constructing the Deque */                                                                 \
    void clear_deque(Deque_##t *deq) {                                                                                                          \
        deq->list_elements = 0;                                                                                                                 \
        deq->list_front_index = 0;                                                                                                              \
        deq->list_back_index = 0;                                                                                                               \
    }                                                                                                                                           \
    /* Function to deallocate all the heap memory allocations for Deque List */                                                                 \
    void dtor_deque(Deque_##t *deq) {                                                                                                           \
        delete[] deq->t##_list;                                                                                                                 \
        deq->t##_list = nullptr;                                                                                                                \
    }                                                                                                                                           \
    /* Function to sort elements of Deque List according to respective compare functions */                                                     \
    void sort_list(Deque_##t *deq, Deque_##t##_Iterator begin, Deque_##t##_Iterator end) {                                                      \
        int i, begin_index = begin.deque_##t##_index;                                                                                           \
        int end_index = end.deque_##t##_index;                                                                                                  \
        int sort_list_size = end_index - begin_index;                                                                                           \
        t *sorted_list = new t[sort_list_size];                                                                                                 \
        /* Logic to copy elements according to passed begin and end iterators */                                                                \
        for (i = 0; i < sort_list_size; i++) {                                                                                                  \
            sorted_list[i] = begin.deref(&begin);                                                                                               \
            begin.inc(&begin);                                                                                                                  \
        }                                                                                                                                       \
        /* Sorting of elements using C++ standard library std::sort(array, size, compare) */                                                    \
        sort(sorted_list, sorted_list + sort_list_size, deq->compare_elements);                                                                 \
        /* Copying back the sorted elements in the original Deque at respective indexes */                                                      \
        int b_index = begin_index, index = 0;                                                                                                   \
        for (i = 0; i < sort_list_size; i++) {                                                                                                  \
            index = deq->list_front_index - b_index;                                                                                            \
            if (index < 0) {                                                                                                                    \
                index += deq->list_capacity;                                                                                                    \
            }                                                                                                                                   \
            deq->t##_list[index] = sorted_list[i];                                                                                              \
            b_index++;                                                                                                                          \
        }                                                                                                                                       \
        delete[] sorted_list;                                                                                                                   \
    }                                                                                                                                           \
    /* Function to construct the Deque List structure for the first time */                                                                     \
    void Deque_##t##_ctor(Deque_##t *deq, bool (*compare_lists)(const t &o1, const t &o2)) {                                                    \
        /* Memory allocation for myClass_list with static size */                                                                               \
        deq->t##_list = new t[LIST_CAPACITY];                                                                                                   \
        /* Logic if Heap memory allocated correctly */                                                                                          \
        if (deq->t##_list != nullptr) {                                                                                                         \
            strcpy(deq->type_name, ("Deque_" #t));                                                                                              \
            deq->list_front_index = 0;                                                                                                          \
            deq->list_back_index = 0;                                                                                                           \
            deq->list_capacity = LIST_CAPACITY;                                                                                                 \
            deq->list_elements = 0;                                                                                                             \
            /* Initialization of function pointers */                                                                                           \
            deq->size = list_size;                                                                                                              \
            deq->empty = list_empty;                                                                                                            \
            deq->push_back = &push_back_deque;                                                                                                  \
            deq->push_front = &push_front_deque;                                                                                                \
            deq->pop_back = &pop_back_deque;                                                                                                    \
            deq->pop_front = &pop_front_deque;                                                                                                  \
            deq->front = &front_deque;                                                                                                          \
            deq->back = &back_deque;                                                                                                            \
            deq->at = &at_deque;                                                                                                                \
            deq->begin = &begin_deque;                                                                                                          \
            deq->end = &end_deque;                                                                                                              \
            deq->clear = &clear_deque;                                                                                                          \
            deq->dtor = &dtor_deque;                                                                                                            \
            deq->compare_elements = compare_lists;                                                                                              \
            deq->sort = sort_list;                                                                                                              \
        }                                                                                                                                       \
    }

#endif
