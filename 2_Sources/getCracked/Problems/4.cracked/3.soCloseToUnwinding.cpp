// So close to unwinding.

// Observe the code snippet below. What is printed?

// Headers

struct A {
    A(char c) : c_(c) {}
    ~A() { std::cout << c_; }
    char c_;
};

struct Y 
{ 
    ~Y() noexcept(false) 
    { 
        throw std::runtime_error("getcracked"); 
    } 
};

A foo() 
{
    try {
        A a('a');
        Y y;
        A b('b');
        return { 'c' }; // #1
    } catch (...) {
    }
    return { 'd' }; // #2
}

int main()
{
    foo();
}

/*
bcad

This is adapted from the example in §[except.ctor]¶2 of the C++ standard regarding stack unwinding:

    Each object with automatic storage duration is destroyed if it has been constructed but not yet destroyed since the try block was entered. If an exception is thrown during the destruction of temporaries or local variables for a return statement (§[stmt.return]), the destructor for the returned object (if any) is also invoked. Objects are destroyed in the reverse order of their construction.

At #1, the returned object of type A is first constructed. Next, the local variable b is destroyed (§[stmt.jump]). Then, destroying y triggers stack unwinding, which leads to the destruction of the returned object and finally the local variable a. The returned object is constructed again at #2.

Note: As of this writing, GCC, Clang, and MSVC do not fully conform to this specified behavior. Instead, they print bacd, bad, and bad, respectively. So, if you tried to cheat by running this code, you definitely got it wrong.
*/