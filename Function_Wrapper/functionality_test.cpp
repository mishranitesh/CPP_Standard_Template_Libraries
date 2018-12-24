#include "function.hpp"
#include <iostream>
#include <cassert>

// Free Function Definition
int one_hundred_ret_func() {
    return 100;
}

// Functor implementation
struct two_hundred_ret_functor {
    int operator()() {
        return 200;
    }
};

// Recursive Free Function Definition
int sum_range_func(int num1, int num2) {
    assert(num1 <= num2);
    return (num1 < num2) ? num1 + sum_range_func(num1 + 1, num2) : num2;
}

// Lambda Function Definition
auto three_hundred_ret_lambda_func = []() {
    return 300;
};

// Main function
int main() {

    {
        // Test default construction
        nm::Function<int()> default_constructed;

        // Test value construction with a free-function
        nm::Function<int()> ret_one_hundred(one_hundred_ret_func);

        // Test value construction with a functor
        nm::Function<int()> ret_two_hundred_functor(two_hundred_ret_functor{});

        // Test value construction with a lambda-function
        nm::Function<int()> ret_three_hundred_lambda(three_hundred_ret_lambda_func);

        // Test function operator on default constructed
        int test_val = 30;
        try {
            default_constructed();
        } catch (nm::BadFunctionCall &bfc) {
            // Code should come here and test_val value will be changed for next assert
            test_val += 10;
        }
        assert(test_val == 40);

        // Test function operator on free-function target, also test that results are correct
        assert(ret_one_hundred() == one_hundred_ret_func());

        // Test function operator on functor target, also test that results are correct
        assert(ret_two_hundred_functor() == two_hundred_ret_functor{}());

        // Test function operator on lambda target, also test that results are correct
        assert(ret_three_hundred_lambda() == three_hundred_ret_lambda_func());

        {
            // Test assignment from Function
            nm::Function<int()> temp;
            temp = ret_one_hundred;
            assert(temp() == one_hundred_ret_func());
        }

        {
            // Test assignment from free-function
            nm::Function<int()> temp;
            temp = one_hundred_ret_func;
            assert(temp() == one_hundred_ret_func());
        }

        {
            // Test assignment from Function containing functor
            nm::Function<int()> temp;
            temp = ret_two_hundred_functor;
            assert(temp() == two_hundred_ret_functor{}());
        }

        {
            // Test assignment from functor
            nm::Function<int()> temp;
            two_hundred_ret_functor functor;
            temp = functor;
            assert(temp() == two_hundred_ret_functor{}());
        }

        {
            // Test assignment from Function containing lambda
            nm::Function<int()> temp;
            temp = ret_three_hundred_lambda;
            assert(temp() == three_hundred_ret_lambda_func());
        }

        {
            // Test assignment from lambda
            nm::Function<int()> temp;
            temp = three_hundred_ret_lambda_func;
            assert(temp() == three_hundred_ret_lambda_func());
        }

        // Test that it is using value semantics.
        {
            struct Functor {
                int operator()() {
                    return i++;
                }

                int i = 0;
            } functor;

            nm::Function<int()> f_temp{functor};
            assert(f_temp() == 0);
            assert(f_temp() == 1);
            assert(f_temp() == 2);
            assert(functor.i == 0);
        }

        {
            // Test equality operators
            assert((!ret_one_hundred.operator bool()) == (ret_one_hundred == nullptr));
            assert((!ret_one_hundred.operator bool()) == (nullptr == ret_one_hundred));

            // Test equality operators with a default constructed object
            nm::Function<void(void)> temp;
            assert((!temp.operator bool()) == (temp == nullptr));
            assert((!temp.operator bool()) == (nullptr == temp));
        }

        {
            // Test inequality operators
            assert(ret_one_hundred.operator bool() == (ret_one_hundred != nullptr));
            assert(ret_one_hundred.operator bool() == (nullptr != ret_one_hundred));

            // Test inequality operators with a default constructed object
            nm::Function<void()> temp;
            assert(!(temp != nullptr));
            assert(!(nullptr != temp));
        }

        {
            nm::Function<int()> temp(ret_one_hundred);
            assert(ret_one_hundred() == temp());
            temp = ret_two_hundred_functor;
            assert(ret_two_hundred_functor() == temp());
            temp = ret_three_hundred_lambda;
            assert(ret_three_hundred_lambda() == temp());
        }

        {
            // Test a function that takes arguments
            nm::Function<int(int, int)> sum_range(sum_range_func);
            assert(sum_range_func(20, 25) == sum_range(20, 25));
        }

        {
            // Test a recursive lambda that captures a value from the surrounding scope
            const int num1 = 25;
            nm::Function<int(int)> sum_range = [&sum_range](int num2) -> int {
                assert(num1 <= num2);
                return (num1 == num2) ? num2 : num2 + sum_range(num2 - 1);
            };

            assert(sum_range(50) == sum_range_func(25, 50));
        }
    }

    std::cout << "\nTEST PASSED SUCCESSFULLY ... !!\n" << std::endl;
}