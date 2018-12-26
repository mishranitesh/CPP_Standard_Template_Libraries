#include "sharedptr.hpp"
#include <iostream>
#include <cassert>

class Base {
protected:
    Base() : derived_destructor_called(false) {
        std::cout << "Base::Base()" << std::endl;
    }

    ~Base() {
        std::cout << "Base::~Base()" << std::endl;
        assert(derived_destructor_called);
    }

    bool derived_destructor_called;

private:
    Base(const Base &);             // Disallow.
    Base &operator=(const Base &);  // Disallow.
};

class Derived : public Base {
public:
    friend int main(int argc, char *argv[]);

    ~Derived() {
        std::cout << "Derived::~Derived()" << std::endl;
        derived_destructor_called = true;
    }

    int value;

private:
    Derived() {}

    Derived(const Derived &);               // Disallow.
    Derived &operator=(const Derived &);    // Disallow.
};

class Base_polymorphic {
protected:
    Base_polymorphic() {
        std::cout << "Base_polymorphic::Base_polymorphic()" << std::endl;
    }

    virtual ~Base_polymorphic() {
        std::cout << "Base_polymorphic::~Base_polymorphic()" << std::endl;
    }

private:
    Base_polymorphic(const Base_polymorphic &);             // Disallow.
    Base_polymorphic &operator=(const Base_polymorphic &);  // Disallow.
};

class Derived_polymorphic : public Base_polymorphic {
public:
    friend int main(int argc, char *argv[]);

private:
    Derived_polymorphic() {}
    Derived_polymorphic(const Derived_polymorphic &);               // Disallow.
    Derived_polymorphic &operator=(const Derived_polymorphic &);    // Disallow.
};

class Derived2_polymorphic : public Base_polymorphic {
private:
    Derived2_polymorphic() {}

    Derived2_polymorphic(const Derived2_polymorphic &);             // Disallow.
    Derived2_polymorphic &operator=(const Derived2_polymorphic &);  // Disallow.
};

int main(int argc, char *argv[]) {

    // Test deleting through original class.
    {
        // Base created directly with Derived *.
        {
            nm::SharedPtr<Base> sp(new Derived);
            {
                // Test copy constructor.
                nm::SharedPtr<Base> sp2(sp);
            }
        }
        // Base assigned from nm::SharedPtr<Derived>.
        {
            nm::SharedPtr<Base> sp2;
            {
                nm::SharedPtr<Derived> sp(new Derived);
                // Test template copy constructor.
                nm::SharedPtr<Base> sp3(sp);
                sp2 = sp;
                sp2 = sp2;
            }
        }
    }

    // Test copy constructor and template copy constructor.
    {
        {
            nm::SharedPtr<Derived> sp(new Derived);
            nm::SharedPtr<Derived> sp2(sp);
        }
        {
            nm::SharedPtr<Derived> sp(new Derived);
            nm::SharedPtr<Base> sp2(sp);
        }
    }

    // Test assignment operator and template assignment operator.
    {
        {
            nm::SharedPtr<Derived> sp(new Derived);
            nm::SharedPtr<Derived> sp2;
            sp2 = sp;
            sp = sp; // Self assignment.
        }
        {
            nm::SharedPtr<Derived> sp(new Derived);
            nm::SharedPtr<Base> sp2;
            sp2 = sp;
            sp2 = sp2; // Self assignment.
            sp2 = sp;
            sp = sp;
        }
    }

    // Test reset.
    {
        {
            nm::SharedPtr<Derived> sp(new Derived);
            nm::SharedPtr<Base> sp2;
            sp2 = sp;
            sp2 = sp2;
            sp.reset();
            sp.reset(new Derived);
            nm::SharedPtr<Base> sp3(sp2);
        }
    }

    // Test * operator.
    {
        nm::SharedPtr<Derived> sp(new Derived);
        (*sp).value = 1111;

        nm::SharedPtr<const Derived> sp2(sp);
        int i = (*sp2).value;
        assert(i == 1111);
    }

    // Test -> operator.
    {
        nm::SharedPtr<Derived> sp(new Derived);
        sp->value = 2222;

        nm::SharedPtr<const Derived> sp2(sp);
        int i = sp2->value;
        assert(i == 2222);
    }

    // Test get() function.
    {
        nm::SharedPtr<Derived> sp(new Derived);
        sp.get()->value = 3333;

        nm::SharedPtr<const Derived> sp2(sp);
        int i = sp2.get()->value;
        assert(i == 3333);
    }

    // Test bool.
    {
        nm::SharedPtr<Derived> sp(new Derived);
        assert(sp);
        sp.reset();
        assert(!sp);

        nm::SharedPtr<Derived> sp2;
    }

    // Test == operator.
    {
        nm::SharedPtr<Derived> sp(new Derived);
        nm::SharedPtr<Derived> sp2(sp);
        nm::SharedPtr<Derived> sp3;

        assert(sp2 == sp);
        assert(!(sp2 == sp3));
        sp3.reset(new Derived);
        assert(!(sp2 == sp3));
    }

    // Test nm::static_pointer_cast() template function.
    {
        nm::SharedPtr<Derived> sp(new Derived);
        nm::SharedPtr<Base> sp2(sp);

        nm::SharedPtr<Derived> sp3(nm::static_pointer_cast<Derived>(sp2));
    }

    // Test nm::dynamic_pointer_cast() template function.
    {
        nm::SharedPtr<Derived_polymorphic> sp(new Derived_polymorphic);
        nm::SharedPtr<Base_polymorphic> sp2(sp);

        nm::SharedPtr<Derived_polymorphic> sp3(nm::dynamic_pointer_cast<Derived_polymorphic>(sp2));
        nm::SharedPtr<Derived_polymorphic> sp4(nm::static_pointer_cast<Derived_polymorphic>(sp2));
        nm::SharedPtr<Derived2_polymorphic> sp5(nm::dynamic_pointer_cast<Derived2_polymorphic>(sp2));
        assert(!sp5);
    }

    // Test const assignment.
    {
        nm::SharedPtr<const Derived> sp_const(new Derived);
        nm::SharedPtr<Derived> sp(new Derived);
        sp_const = sp;
    }

    std::cout << "\nTEST PASSED SUCCESSFULLY ... !!\n" << std::endl;
}