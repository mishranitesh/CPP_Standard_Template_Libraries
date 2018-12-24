#ifndef NITESH_FUNCTION_HPP
#define NITESH_FUNCTION_HPP

#include <iostream>
#include <exception>

namespace nm {

    /*
     * Custom Exception Class -> invoked when _invokable is pointing to nullptr
     */
    class BadFunctionCall : public std::exception {
    public:
        const char *what() const throw() {
            return "BadFunctionCall exception\n";
        }
    };

    /*
     * Undefined class declaration
     */
    template<typename>
    class Function;

    /*
     * Template Function class definition
     * Using "call signature" --> ResultType(ArgumentTypes...)
     */
    template<typename ResultType, typename ... ArgumentTypes>
    class Function<ResultType(ArgumentTypes...)> {

    private:

        /*
         * Inner class IInvokable (working as interface with pure virtual function)
         */
        class IInvokable {
        public:
            // Member variable to hold count of references
            std::size_t _ref_counter;

            // Default Constructor
            IInvokable() : _ref_counter{0} {}

            // Default Destructor
            virtual ~IInvokable() = default;

            // Pure virtual function to be implemented by derived class
            virtual ResultType call(ArgumentTypes...) = 0;
        };

        /*
         * Inner class Invokable inheriting IInvokable class
         */
        template<typename T>
        class Invokable : public IInvokable {
        public:
            // Member variable to hold function type
            T _function_type;

            // Default Constructor
            Invokable() = delete;

            // Value constructor
            explicit Invokable(const T &function_type) : _function_type{function_type} {}

            // Destructor
            ~Invokable() override = default;

            // Method to be invoked while operator() is called
            ResultType call(ArgumentTypes... args) override {
                return _function_type(args...);
            }
        };

        // Member variable as pointer to IInvokable class
        IInvokable *_invokable;

    public:
        // Default constructor
        Function() : _invokable{nullptr} {}

        // Template value constructor
        template<typename FunctionType>
        Function(FunctionType func_type) : _invokable{nullptr} {
            // New memory allocation for Invokable object creation with FunctionType
            _invokable = new Invokable<FunctionType>(func_type);
            ++(_invokable->_ref_counter);
        }

        // Copy constructor
        Function(const Function &func) : _invokable{nullptr} {
            _invokable = func._invokable;
            ++(_invokable->_ref_counter);
        }

        // Copy assignment operator
        Function &operator=(Function &func) {
            // Self-assignment check
            if (this == &func) return *this;

            // Memory deallocate if reference counter condition satisfies
            if (_invokable != nullptr) {
                if (_invokable->_ref_counter == 1) {
                    delete _invokable;
                } else {
                    --(_invokable->_ref_counter);
                }
                _invokable = nullptr;
            }

            // Assignment with passed function argument
            if (func._invokable) {
                _invokable = func._invokable;
                ++(_invokable->_ref_counter);
            }

            return *this;
        }

        // Template copy assignment operator
        template<typename FunctionType>
        Function &operator=(const FunctionType func_type) {
            // Memory deallocate if reference counter condition satisfies
            if (_invokable != nullptr) {
                if (_invokable->_ref_counter == 1) {
                    delete _invokable;
                } else {
                    --(_invokable->_ref_counter);
                }
                _invokable = nullptr;
            }

            // New memory allocation with passed function type argument
            _invokable = new Invokable<FunctionType>(func_type);
            ++(_invokable->_ref_counter);

            return *this;
        }

        // Destructor
        ~Function() {
            // Memory deallocate if reference counter condition satisfies
            if (_invokable != nullptr) {
                if (_invokable->_ref_counter == 1) {
                    delete _invokable;
                } else {
                    --(_invokable->_ref_counter);
                }
            }
        }

        // Functor operator () overloading
        ResultType operator()(ArgumentTypes... args) {
            if (_invokable == nullptr) throw BadFunctionCall();
            return _invokable->call(args...);
        }

        // Conversion function implementation
        explicit operator bool() const {
            return (_invokable != nullptr);
        }
    };

    // Free functions to compare function objects with nullptr_t
    template<typename ResultType, typename... ArgumentTypes>
    bool operator==(const Function<ResultType(ArgumentTypes...)> &f, std::nullptr_t) {
        return !(f != nullptr);
    }

    template<typename ResultType, typename... ArgumentTypes>
    bool operator!=(const Function<ResultType(ArgumentTypes...)> &f, std::nullptr_t) {
        return bool(f);
    }

    template<typename ResultType, typename... ArgumentTypes>
    bool operator==(std::nullptr_t, const Function<ResultType(ArgumentTypes...)> &f) {
        return !(f != nullptr);
    }

    template<typename ResultType, typename... ArgumentTypes>
    bool operator!=(std::nullptr_t, const Function<ResultType(ArgumentTypes...)> &f) {
        return bool(f);
    }
}

#endif
