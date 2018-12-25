#ifndef NITESH_ARRAY_HPP
#define NITESH_ARRAY_HPP

#include <iostream>
#include <chrono>

namespace nm {

    /*
     * Class to represent custom int user defined data type
     */
    class Custom_Int {
        friend std::ostream &operator<<(std::ostream &os, const Custom_Int &mi) {
            return os << mi.i;
        }
    public:
        Custom_Int() { count++; }
        Custom_Int(const Custom_Int &mi) : i(mi.i) { count++; }
        Custom_Int(int i) : i(i) { count++; }
    private:
        int i;
    public:
        static std::size_t count;
    };
    std::size_t Custom_Int::count;

    /*
     * Class to represent Array to perform RValue operations
     */
    class Array {
    public:
        /* Friend function for free overloaded output operator function */
        friend std::ostream &operator<<(std::ostream &os, const Array &arr);

        Array() : _custom_int_arr{0}, _arr_size{0} {}       // Default ctor
        Array(std::initializer_list<int>);                  // Initializer list ctor
        explicit Array(std::size_t);                        // Explicit conversion ctor
        Array(const Array &);                               // Copy ctor
        Array(Array &&);                                    // Move ctor
        ~Array();                                           // Default dtor
        Array &operator=(const Array &);                    // Copy assignment ctor
        Array &operator=(Array &&);                         // Move assignment ctor
        static void move_performance_test();                // Function to test performance

    private:
        /* Member variables */
        Custom_Int **_custom_int_arr;
        std::size_t _arr_size;
    };

    // Value constructor using initializer_list of int
    Array::Array(std::initializer_list<int> init_list) {
        // Allocate sufficient space for Custom_Int objects in heap
        _custom_int_arr = new Custom_Int *[init_list.size()];
        _arr_size = init_list.size();

        // Traverse through initializer list and insert the objects of Custom_Int
        int index = 0;
        const int *existing_value = init_list.begin();
        while (existing_value != init_list.end()) {
            _custom_int_arr[index] = new Custom_Int(*existing_value);
            ++index;
            ++existing_value;
        }
    }

    // Conversion constructor
    Array::Array(std::size_t size) {
        // Allocate sufficient space for Custom_Int objects in heap
        _custom_int_arr = new Custom_Int *[size];
        _arr_size = size;

        // Insert Custom_Int objects
        for (std::size_t i = 0; i < size; ++i) {
            _custom_int_arr[i] = new Custom_Int((int) (i + 1)); // C-style casting from size_t to int
        }
    }

    // Copy constructor (using lvalue reference)
    Array::Array(const Array &existing_arr) {
        // Allocate sufficient space for Custom_Int objects in heap
        _custom_int_arr = new Custom_Int *[existing_arr._arr_size];
        _arr_size = existing_arr._arr_size;

        // Traverse through existing array and insert
        for (std::size_t i = 0; i < existing_arr._arr_size; ++i) {
            _custom_int_arr[i] = new Custom_Int(*existing_arr._custom_int_arr[i]);
        }
    }

    // Move constructor (using rvalue reference)
    Array::Array(Array &&existing_arr) {
        // Value Semantic pointers assignment
        _custom_int_arr = existing_arr._custom_int_arr;
        _arr_size = existing_arr._arr_size;
        existing_arr._custom_int_arr = nullptr;
        existing_arr._arr_size = 0;

        // Alternate implementation with some memory allocation
        /*// Allocate sufficient space for Custom_Int objects in heap
        _custom_int_arr = new Custom_Int*[existing_arr._arr_size];
        _arr_size = existing_arr._arr_size;

        // Traverse and steal the Custom_Int objects from existing array
        for (std::size_t i = 0; i < existing_arr._arr_size; ++i) {
            _custom_int_arr[i] = existing_arr._custom_int_arr[i];
            existing_arr._custom_int_arr[i] = nullptr;
        }
        existing_arr._arr_size = 0;*/
    }

    // Copy assignment operator (using lvalue reference)
    Array &Array::operator=(const Array &existing_arr) {
        // Self-assignment check
        if (this == &existing_arr) return *this;

        // Delete already present Custom_Int objects from array
        for (std::size_t i = 0; i < _arr_size; ++i) {
            delete _custom_int_arr[i];
        }
        delete[] _custom_int_arr;
        _arr_size = 0;

        // Allocate sufficient space for Custom_Int objects in heap
        _custom_int_arr = new Custom_Int *[existing_arr._arr_size];
        _arr_size = existing_arr._arr_size;

        // Traverse through existing array and insert
        for (std::size_t i = 0; i < existing_arr._arr_size; ++i) {
            _custom_int_arr[i] = new Custom_Int(*existing_arr._custom_int_arr[i]);
        }

        return *this;
    }

    // Move assignment operator (using rvalue reference)
    Array &Array::operator=(Array &&existing_arr) {
        // Self-assignment check
        if (this == &existing_arr) return *this;

        // Delete already present Custom_Int objects from array
        for (std::size_t i = 0; i < _arr_size; ++i) {
            delete _custom_int_arr[i];
        }
        delete[] _custom_int_arr;
        _arr_size = 0;

        // Value Semantic pointers assignment
        _custom_int_arr = existing_arr._custom_int_arr;
        _arr_size = existing_arr._arr_size;
        existing_arr._custom_int_arr = nullptr;
        existing_arr._arr_size = 0;

        // Alternate implementation with some memory allocation
        /*// Allocate sufficient space for Custom_Int objects in heap
        _custom_int_arr = new Custom_Int*[existing_arr._arr_size];
        _arr_size = existing_arr._arr_size;

        // Traverse and steal the Custom_Int objects from existing array
        for (std::size_t i = 0; i < existing_arr._arr_size; ++i) {
            _custom_int_arr[i] = existing_arr._custom_int_arr[i];
            existing_arr._custom_int_arr[i] = nullptr;
        }
        existing_arr._arr_size = 0;*/

        return *this;
    }

    // Destructor implementation
    Array::~Array() {
        // Deallocate the heap space
        for (std::size_t i = 0; i < _arr_size; ++i) {
            delete _custom_int_arr[i];
        }
        delete[] _custom_int_arr;
        _arr_size = 0;
    }

    // Output operator << overloading
    std::ostream &operator<<(std::ostream &os, const Array &arr) {
        for (std::size_t i = 0; i < arr._arr_size; ++i) {
            os << *arr._custom_int_arr[i];
            if (i + 1 != arr._arr_size) {
                os << ", ";
            }
        }
        return os;
    }

    // Performance comparison between Copy ctor/ assignment operator and Move ctor/ assignment operator
    void Array::move_performance_test() {

        // Standard library use for chrono time
        using Milli = std::chrono::duration<double, std::ratio<1, 1000>>;
        using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
        using namespace std::chrono;

        TimePoint start, end;
        Milli elapsed;
        std::size_t size = 100000;

        nm::Array main_arr_1(size); // using conversion constructor

        std::cout << "\n\n---------------------------------- PERFORMANCE TESTING (Copy vs Move) ----------------------------------\n" << std::endl;

        // Copy constructor
        start = system_clock::now();
        nm::Array copy_arr{main_arr_1};
        end = system_clock::now();
        elapsed = end - start;
        std::cout << "\'COPY CONSTRUCTOR\' :==>> \t\t Size of array " << size << " has taken --> \t" << elapsed.count() << " milliseconds."
                  << std::endl;

        // Move constructor
        start = system_clock::now();
        nm::Array move_arr{std::move(main_arr_1)};
        end = system_clock::now();
        elapsed = end - start;
        std::cout << "\'MOVE CONSTRUCTOR\' :==>> \t\t Size of array " << size << " has taken --> \t" << elapsed.count() << " milliseconds.\n\n"
                  << std::endl;


        nm::Array main_arr_2(size); // using conversion constructor

        // Copy assignment constructor
        nm::Array copy_a_arr{1};
        start = system_clock::now();
        copy_a_arr = main_arr_2;
        end = system_clock::now();
        elapsed = end - start;
        std::cout << "\'COPY ASSIGNMENT OPERATOR\' :==>> \t Size of array " << size << " has taken --> \t" << elapsed.count() << " milliseconds."
                  << std::endl;

        // Move assignment constructor
        nm::Array move_a_arr{1};
        start = system_clock::now();
        move_a_arr = std::move(main_arr_2);
        end = system_clock::now();
        elapsed = end - start;
        std::cout << "\'MOVE ASSIGNMENT OPERATOR\' :==>> \t Size of array " << size << " has taken --> \t" << elapsed.count() << " milliseconds."
                  << std::endl;

        std::cout << "\n--------------------------------------------------------------------------------------------------------\n" << std::endl;
    }
}

#endif