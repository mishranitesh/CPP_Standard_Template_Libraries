#ifndef NITESH_SHARED_PTR_HPP
#define NITESH_SHARED_PTR_HPP

#include <iostream>
#include <pthread.h>
#include <mutex>

namespace nm {

    // Declaration of C++11 standard mutex
    std::mutex mutex_lock;

    // SharedPtr class prototype
    template<typename T>
    class SharedPtr;

    /*
     * Class to hold count of object references
     */
    class Reference_Counter {
    public:
        template<typename T>
        friend
        class SharedPtr;     // Friend class SharedPtr
        template<typename T1, typename U1>
        friend SharedPtr<T1> static_pointer_cast(const SharedPtr<U1> &sp);

        template<typename T1, typename U1>
        friend SharedPtr<T1> dynamic_pointer_cast(const SharedPtr<U1> &sp);

        Reference_Counter() : _ref_counter{0} {}    // Default Constructor
        virtual ~Reference_Counter() = default;     // Default Destructor

    private:
        std::size_t _ref_counter;
    };

    /*
     * Template subclass to hold reference of the respective object
     */
    template<typename T>
    class Reference_Pointer : public Reference_Counter {
    public:
        template<typename U>
        friend
        class SharedPtr;     // Friend class SharedPtr
        template<typename T1, typename U1>
        friend SharedPtr<T1> static_pointer_cast(const SharedPtr<U1> &sp);

        template<typename T1, typename U1>
        friend SharedPtr<T1> dynamic_pointer_cast(const SharedPtr<U1> &sp);

        Reference_Pointer() : _ref_ptr{nullptr} {}      // Default Constructor
        Reference_Pointer(T *ref_ptr) : _ref_ptr{ref_ptr} {} // Parameter Constructor
        ~Reference_Pointer() {  // Default Destructor
            delete _ref_ptr;
            _ref_ptr = nullptr;
        }

    private:
        T *_ref_ptr;
    };

    /*
     * Template SharedPtr class - Smart Pointer implementation
     */
    template<typename T>
    class SharedPtr {

    public:

        /*
         * Class member variables
         */
        Reference_Counter *_ref_counter_ptr;
        T *_ref_ptr_helper;

        /*
         * Constructor to initialize smart pointer to point to nullptr
         */
        SharedPtr() : _ref_counter_ptr{nullptr}, _ref_ptr_helper{nullptr} {}

        /*
         * Conversion constructor (explicit) to initialize smart pointer with given object
         * and reference count is initialized to one
         */
        template<typename U>
        explicit SharedPtr(U *obj_ptr);

        /*
         * Copy Constructor
         * If shared_ptr is not null, then reference count of the managed object is incremented
         */
        SharedPtr(const SharedPtr &shared_ptr);

        /*
         * Template Copy Constructor
         * If U* is not implicitly convertible to T*, compile-time error
         */
        template<typename U>
        SharedPtr(const SharedPtr<U> &shared_ptr);

        /*
         * Move Constructor
         * Move the managed object from the given smart pointer
         * Reference count must remain unchanged and shared_ptr must be nullptr
         */
        SharedPtr(SharedPtr &&shared_ptr);

        /*
         * Template Move Constructor
         * Must work if U* is implicitly convertible to T* otherwise syntax error
         */
        template<typename U>
        SharedPtr(SharedPtr<U> &&shared_ptr);

        /*
         * Copy Assignment Operator
         * Decrement reference count of current object (if any)
         * Increment reference count of the given object
         */
        SharedPtr &operator=(const SharedPtr &shared_ptr);

        /*
         * Template Copy Assignment Operator
         * If U * is not implicitly convertible to T*, syntax error
         */
        template<typename U>
        SharedPtr<T> &operator=(const SharedPtr<U> &shared_ptr);

        /*
         * Move Assignment Operator
         * Reference count must remain unchanged and p must be nullptr
         */
        SharedPtr &operator=(SharedPtr &&shared_ptr);

        /*
         * Template Move Assignment Operator
         * If U * is not implicitly convertible to T*, syntax error
         */
        template<typename U>
        SharedPtr &operator=(SharedPtr<U> &&shared_ptr);

        /*
         * Destructor
         * Decrement reference count of managed object
         * If the reference count is zero, delete the object
         */
        ~SharedPtr();

        /*
         * Smart pointer is set to point to the nullptr
         * Reference count for the currently pointed to object (if any) is decremented
         */
        void reset();

        /*
         * Replace owned resource with another pointer
         * If the owned resource has no other references, it is deleted
         * If ptr has been associated with some other smart pointer, undefined behavior
         */
        template<typename U>
        void reset(U *ref_ptr);

        /*
         * Returns a pointer to the owned object
         */
        T *get() const {
            return _ref_ptr_helper;
        }

        /*
         * A reference to the pointed-to object is returned
         */
        T &operator*() const {
            return *_ref_ptr_helper;
        }

        /*
         * The pointer is returned
         */
        T *operator->() const {
            return _ref_ptr_helper;
        }

        /*
         * Conversion function
         * Returns true if the SharedPtr is not null
         */
        explicit operator bool() const {
            return (_ref_ptr_helper != nullptr);
        }

        /*
         * Friend function declaration for free functions
         */
        template<typename T1, typename T2>
        friend bool operator==(const SharedPtr<T1> &, const SharedPtr<T2> &);

        template<typename T1, typename T2>
        friend bool operator!=(const SharedPtr<T1> &, const SharedPtr<T2> &);

        template<typename T1>
        friend bool operator==(const SharedPtr<T1> &, std::nullptr_t);

        template<typename T1>
        friend bool operator!=(const SharedPtr<T1> &, std::nullptr_t);

        template<typename T1>
        friend bool operator==(std::nullptr_t, const SharedPtr<T1> &);

        template<typename T1>
        friend bool operator!=(std::nullptr_t, const SharedPtr<T1> &);

        template<typename T1, typename U1>
        friend SharedPtr<T1> static_pointer_cast(const SharedPtr<U1> &sp);

        template<typename T1, typename U1>
        friend SharedPtr<T1> dynamic_pointer_cast(const SharedPtr<U1> &sp);
    };

    /* Constructors, Assignment operators and Member methods implementations */

    // Conversion constructor
    template<typename T>
    template<typename U>
    SharedPtr<T>::SharedPtr(U *obj_ptr) : _ref_ptr_helper{obj_ptr} {
        _ref_counter_ptr = new Reference_Pointer<U>(obj_ptr);
        mutex_lock.lock();
        ++(_ref_counter_ptr->_ref_counter);
        mutex_lock.unlock();
    }

    // Copy constructor
    template<typename T>
    SharedPtr<T>::SharedPtr(const SharedPtr &shared_ptr) : _ref_counter_ptr{nullptr}, _ref_ptr_helper{nullptr} {
        if (shared_ptr) {
            _ref_ptr_helper = shared_ptr._ref_ptr_helper;
            _ref_counter_ptr = shared_ptr._ref_counter_ptr;
            mutex_lock.lock();
            ++(_ref_counter_ptr->_ref_counter);
            mutex_lock.unlock();
        }
    }

    // Template Copy Constructor
    template<typename T>
    template<typename U>
    SharedPtr<T>::SharedPtr(const SharedPtr<U> &shared_ptr) : _ref_counter_ptr{nullptr}, _ref_ptr_helper{nullptr} {
        if (shared_ptr) {
            _ref_ptr_helper = shared_ptr._ref_ptr_helper;
            _ref_counter_ptr = shared_ptr._ref_counter_ptr;
            mutex_lock.lock();
            ++(_ref_counter_ptr->_ref_counter);
            mutex_lock.unlock();
        }
    }

    // Move Constructor
    template<typename T>
    SharedPtr<T>::SharedPtr(SharedPtr<T> &&shared_ptr) : _ref_counter_ptr{nullptr}, _ref_ptr_helper{nullptr} {
        if (shared_ptr) {
            _ref_ptr_helper = shared_ptr._ref_ptr_helper;
            _ref_counter_ptr = shared_ptr._ref_counter_ptr;
            shared_ptr._ref_ptr_helper = nullptr;
            shared_ptr._ref_counter_ptr = nullptr;
        }
    }

    // Template Move Constructor
    template<typename T>
    template<typename U>
    SharedPtr<T>::SharedPtr(SharedPtr<U> &&shared_ptr) : _ref_counter_ptr{nullptr}, _ref_ptr_helper{nullptr} {
        if (shared_ptr) {
            _ref_ptr_helper = shared_ptr._ref_ptr_helper;
            _ref_counter_ptr = shared_ptr._ref_counter_ptr;
            shared_ptr._ref_ptr_helper = nullptr;
            shared_ptr._ref_counter_ptr = nullptr;
        }
    }

    // Copy Assignment Operator
    template<typename T>
    SharedPtr<T> &SharedPtr<T>::operator=(const SharedPtr<T> &shared_ptr) {
        // Self-assignment condition check
        if (this == &shared_ptr) {
            return *this;
        }

        mutex_lock.lock();
        // Check for reference counter and perform reference deletion accordingly
        if (_ref_counter_ptr != nullptr) {
            if (_ref_counter_ptr->_ref_counter == 1) {
                delete _ref_counter_ptr;
            } else {
                --(_ref_counter_ptr->_ref_counter);
            }
            _ref_counter_ptr = nullptr;
            _ref_ptr_helper = nullptr;
        }
        mutex_lock.unlock();

        // Copy implementation if shared_ptr is not nullptr
        if (shared_ptr) {
            _ref_ptr_helper = shared_ptr._ref_ptr_helper;
            _ref_counter_ptr = shared_ptr._ref_counter_ptr;
            mutex_lock.lock();
            ++(_ref_counter_ptr->_ref_counter);
            mutex_lock.unlock();
        }

        return *this;
    }

    // Template Copy Assignment Operator
    template<typename T>
    template<typename U>
    SharedPtr<T> &SharedPtr<T>::operator=(const SharedPtr<U> &shared_ptr) {
        mutex_lock.lock();
        // No need to check for self-assignment here
        // Check for reference counter and perform reference deletion accordingly
        if (_ref_counter_ptr != nullptr) {
            if (_ref_counter_ptr->_ref_counter == 1) {
                delete _ref_counter_ptr;
            } else {
                --(_ref_counter_ptr->_ref_counter);
            }
            _ref_counter_ptr = nullptr;
            _ref_ptr_helper = nullptr;
        }
        mutex_lock.unlock();

        // Copy implementation if shared_ptr is not nullptr
        if (shared_ptr) {
            _ref_ptr_helper = shared_ptr._ref_ptr_helper;
            _ref_counter_ptr = shared_ptr._ref_counter_ptr;
            mutex_lock.lock();
            ++(_ref_counter_ptr->_ref_counter);
            mutex_lock.unlock();
        }

        return *this;
    }

    // Move Assignment Operator
    template<typename T>
    SharedPtr<T> &SharedPtr<T>::operator=(SharedPtr<T> &&shared_ptr) {
        // Self-assignment condition check
        if (this == &shared_ptr) {
            return *this;
        }

        mutex_lock.lock();
        // Check for reference counter and perform reference deletion accordingly
        if (_ref_counter_ptr != nullptr) {
            if (_ref_counter_ptr->_ref_counter == 1) {
                delete _ref_counter_ptr;
            } else {
                --(_ref_counter_ptr->_ref_counter);
            }
            _ref_counter_ptr = nullptr;
            _ref_ptr_helper = nullptr;
        }
        mutex_lock.unlock();

        // Move implementation if shared_ptr is not nullptr
        if (shared_ptr) {
            _ref_ptr_helper = shared_ptr._ref_ptr_helper;
            _ref_counter_ptr = shared_ptr._ref_counter_ptr;
            shared_ptr._ref_ptr_helper = nullptr;
            shared_ptr._ref_counter_ptr = nullptr;
        }

        return *this;
    }

    // Template Move Assignment Operator
    template<typename T>
    template<typename U>
    SharedPtr<T> &SharedPtr<T>::operator=(SharedPtr<U> &&shared_ptr) {
        mutex_lock.lock();
        // No need to check for self-assignment here
        // Check for reference counter and perform reference deletion accordingly
        if (_ref_counter_ptr != nullptr) {
            if (_ref_counter_ptr->_ref_counter == 1) {
                delete _ref_counter_ptr;
            } else {
                --(_ref_counter_ptr->_ref_counter);
            }
            _ref_counter_ptr = nullptr;
            _ref_ptr_helper = nullptr;
        }
        mutex_lock.unlock();

        // Move implementation if shared_ptr is not nullptr
        if (shared_ptr) {
            _ref_ptr_helper = shared_ptr._ref_ptr_helper;
            _ref_counter_ptr = shared_ptr._ref_counter_ptr;
            shared_ptr._ref_ptr_helper = nullptr;
            shared_ptr._ref_counter_ptr = nullptr;
        }

        return *this;
    }

    // Destructor
    template<typename T>
    SharedPtr<T>::~SharedPtr() {
        mutex_lock.lock();
        // Check for reference counter and perform reference deletion accordingly
        if (_ref_counter_ptr != nullptr) {
            if (_ref_counter_ptr->_ref_counter == 1) {
                _ref_ptr_helper = nullptr;
                delete _ref_counter_ptr;
                _ref_counter_ptr = nullptr;
            } else {
                --(_ref_counter_ptr->_ref_counter);
            }
        }
        mutex_lock.unlock();
    }

    // Reset method
    template<typename T>
    void SharedPtr<T>::reset() {
        mutex_lock.lock();
        // Check for reference counter and perform reference deletion accordingly
        if (_ref_counter_ptr != nullptr) {
            if (_ref_counter_ptr->_ref_counter == 1) {
                _ref_ptr_helper = nullptr;
                delete _ref_counter_ptr;
                _ref_counter_ptr = nullptr;
            } else {
                --(_ref_counter_ptr->_ref_counter);
            }
            _ref_ptr_helper = nullptr;
            _ref_counter_ptr = nullptr;
        }
        mutex_lock.unlock();
    }

    // Template Reset method
    template<typename T>
    template<typename U>
    void SharedPtr<T>::reset(U *ref_ptr) {
        mutex_lock.lock();
        // Check for reference counter and perform reference deletion accordingly
        if (_ref_counter_ptr != nullptr) {
            if (_ref_counter_ptr->_ref_counter == 1) {
                delete _ref_counter_ptr;
            } else {
                --(_ref_counter_ptr->_ref_counter);
            }
            _ref_counter_ptr = nullptr;
            _ref_ptr_helper = nullptr;
        }

        // Allocation for new object
        if (ref_ptr) {
            _ref_ptr_helper = ref_ptr;
            _ref_counter_ptr = new Reference_Pointer<U>(ref_ptr);
            ++(_ref_counter_ptr->_ref_counter);
        }
        mutex_lock.unlock();
    }

    /*
     * True if both points to the same object or if both are nullptr
     * Note that implicit conversions may be applied
     */
    template<typename T1, typename T2>
    bool operator==(const SharedPtr<T1> &shared_ptr_1, const SharedPtr<T2> &shared_ptr_2) {
        if (shared_ptr_1._ref_ptr_helper == nullptr && shared_ptr_2._ref_ptr_helper == nullptr) {
            return true;
        }
        return (shared_ptr_1._ref_ptr_helper == shared_ptr_2._ref_ptr_helper);
    }

    /*
     * True if both points to different objects or one points to nullptr
     */
    template<typename T1, typename T2>
    bool operator!=(const SharedPtr<T1> &shared_ptr_1, const SharedPtr<T2> &shared_ptr_2) {
        return !(shared_ptr_1 == shared_ptr_2);
    }

    /*
     * Compare the SharedPtr against nullptr
     * Return true if shared_ptr is true
     */
    template<typename T>
    bool operator==(const SharedPtr<T> &shared_ptr, std::nullptr_t) {
        return (!shared_ptr);
    }

    /*
     * Compare the SharedPtr against nullptr
     */
    template<typename T>
    bool operator!=(const SharedPtr<T> &shared_ptr, std::nullptr_t null_ptr) {
        return !(shared_ptr == null_ptr);
    }

    /*
     * Compare the SharedPtr against nullptr
     * Return true if shared_ptr is true
     */
    template<typename T>
    bool operator==(std::nullptr_t, const SharedPtr<T> &shared_ptr) {
        return (!shared_ptr);
    }

    /*
     * Compare the SharedPtr against nullptr
     */
    template<typename T>
    bool operator!=(std::nullptr_t null_ptr, const SharedPtr<T> &shared_ptr) {
        return !(null_ptr == shared_ptr);
    }

    /*
     * Convert sp by using static_cast to cast the contained pointer
     * If static_cast cannot be applied to the relevant types, syntax error
     */
    template<typename T, typename U>
    SharedPtr<T> static_pointer_cast(const SharedPtr<U> &shared_ptr) {
        SharedPtr<T> ret_shared_ptr;
        if (shared_ptr) {
            ret_shared_ptr._ref_ptr_helper = static_cast<T *>(shared_ptr._ref_ptr_helper);
            ret_shared_ptr._ref_counter_ptr = shared_ptr._ref_counter_ptr;
            mutex_lock.lock();
            ++(ret_shared_ptr._ref_counter_ptr->_ref_counter);
            mutex_lock.unlock();
        }
        return ret_shared_ptr;
    }

    /*
     * Convert sp by using dynamic_cast to cast the contained pointer
     * If dynamic_cast cannot be applied to the relevant types, syntax error
     * Result in smart pointer to null if dynamic type of the pointer in sp is not T*
     */
    template<typename T, typename U>
    SharedPtr<T> dynamic_pointer_cast(const SharedPtr<U> &shared_ptr) {
        SharedPtr<T> ret_shared_ptr;
        if (shared_ptr) {
            ret_shared_ptr._ref_ptr_helper = dynamic_cast<T *>(shared_ptr._ref_ptr_helper);
            ret_shared_ptr._ref_counter_ptr = shared_ptr._ref_counter_ptr;
            mutex_lock.lock();
            ++(ret_shared_ptr._ref_counter_ptr->_ref_counter);
            mutex_lock.unlock();
        }
        return ret_shared_ptr;
    }

}

#endif