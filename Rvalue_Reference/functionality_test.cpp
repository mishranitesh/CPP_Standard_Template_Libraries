#include "rvalue.hpp"
#include <cassert>

int main() {

    std::size_t before;

    // Creates an array. The contained element should be of type Custom_Int.
    nm::Array a1{1, 2, 3};
    std::cout << "\nTesting ctor: \n    " << a1 << std::endl;

    // Both a2 and a1 should be identical after the copy ctor.
    std::cout << "Testing copy ctor: " << std::endl;
    before = nm::Custom_Int::count;
    nm::Array a2{a1};
    // At least 3 Custom_Int objects should be created.
    assert(nm::Custom_Int::count - before >= 3);
    std::cout << "    " << a2 << std::endl;
    std::cout << "    " << a1 << std::endl;

    std::cout << "Testing move ctor: " << std::endl;
    before = nm::Custom_Int::count;
    nm::Array a3{std::move(a2)};
    // No new Custom_Int objects should be created here.
    assert(nm::Custom_Int::count - before == 0);
    std::cout << "    " << a3 << std::endl;

    std::cout << "Testing copy assignment: " << std::endl;
    nm::Array a4{1};
    before = nm::Custom_Int::count;
    a4 = a1;
    // At least 3 Custom_Int objects should be created.
    assert(nm::Custom_Int::count - before >= 3);
    std::cout << "    " << a4 << std::endl;
    std::cout << "    " << a1 << std::endl;

    std::cout << "Testing move assignment: " << std::endl;
    nm::Array a5{1};
    before = nm::Custom_Int::count;
    a5 = std::move(a4);
    // No new Custom_Int objects should be created here.
    assert(nm::Custom_Int::count - before == 0);
    std::cout << "    " << a5 << std::endl;

    // Test performance of Copy vs Move for same input
    nm::Array::move_performance_test();
}