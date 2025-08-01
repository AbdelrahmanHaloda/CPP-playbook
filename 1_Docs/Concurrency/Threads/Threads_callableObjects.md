### <u>Starting a Thread with a Function Object</u>

**Callable Objects**
In C++, **callable objects** are object that can appear as the left-hand operand of the call operator.

In the context of concurrency, we can use **callable objects** to attach a function to a thread.
 

**Callable Objects can be:-**
- **pointer to function**
- **Function Object (Functor).** 
- **Lambdas (an anonymous inline function)**,

---

### <u>Passing Data to threads</u> 

**There are several ways of passing data to a thread function using "callable objects":-**

**Note:-**
**The following methods represent passing data to threads from the parent thread to the worker thread.**

**1- Pointer to function.** 
Passing a function with no arguments to a thread object is not always efficient because the only way to make data available from within the thread function would be to use global variables - which is definitely not recommendable and also incredibly messy.

**2- Function Objects which are also known as (Functors).**
Passing instances of classes that implement the function-call operator.

**By overloading the operator()**
When you overload the operator(), objects of the class can be called like functions. This is particularly useful for threading because the **std::thread** constructor expects a callable object, which can be a function pointer, a function object (like a functor), or a lambda function.

**Example 1:**
using Functor:
The below code, a class is defined that has an overloaded ()-operator.

```
#include <iostream>
#include <thread>

class Vehicle
{
public:
    Vehicle(int id) : _id(id) {}
    void operator()()
    {
        std::cout << "Vehicle #" << _id << " has been created" << std::endl;
    }

private:
    int _id;
};

int main()
{
    // create thread 
    std::thread t(Vehicle(1)); // C++'s most vexing parse

    // do something in main()
    std::cout << "Finished work in main \n";

    // wait for thread to finish
    t.join();

    return 0;
}
```
**Example 1 o/p:**
```
error: request for member ‘join’ in ‘t’, which is of non-class type ‘std::thread(Vehicle (*)())’
     t.join();
```

The extra parentheses suggested by the compiler avoid what is known as C++'s **"most vexing parse"**, which is a specific form of syntactic ambiguity resolution in the C++ programming language.

The expression was coined by Scott Meyers in 2001, who talks about it in details in his book **"Effective STL"**. The **"most vexing parse"** comes from a rule in C++ that says that **anything that could be considered as a function declaration, the compiler should parse it as a function declaration - even if it could be interpreted as something else.**

The line from Example 1
`std::thread t(Vehicle(1));` is seemingly ambiguous, since it could be interpreted either as

1- A variable definition for variable t of class std::thread, initialized with an anonymous instance of class Vehicle.

2- A function declaration for a function t that returns an object of type std::thread and has a single (unnamed) parameter that is a pointer to function returning an object of type Vehicle.

Most programmers would presumable expect the first case to be true, but the C++ standard requires it to be interpreted as the second - hence the compiler warning.

There are three ways of forcing the compiler to consider the line as the first case, which would create the thread object we want:

**- Add an extra pair of parentheses.**
**- Use copy initialization.**
**- Use uniform initialization with braces.**

**Example 2**
The following code shows all three variants:

```
#include <iostream>
#include <thread>

class Vehicle
{
public:
    Vehicle(int id) : _id(id) {}
    void operator()()
    {
        std::cout << "Vehicle #" << _id << " has been created" << std::endl;
    }

private:
    int _id;
};

int main()
{
    // create thread 
    //std::thread t0(Vehicle()); // C++'s most vexing parse
    
    std::thread t1( (Vehicle(1)) ); // Add an extra pair of parantheses
    
    std::thread t2 = std::thread( Vehicle(2) ); // Use copy initialization
    
    std::thread t3{ Vehicle(3) };// Use uniform initialization with braces

    // do something in main()
    std::cout << "Finished work in main \n";

    // wait for thread to finish
    t1.join();
    t2.join();
    t3.join();

    return 0;
}
```

**Example 2 o/p:**
The put of this code sample shows that all three threads are executed and the Vehicle object is properly initialized:
```
Vehicle #1 has been created
Vehicle #2 has been created
Finished work in main 
Vehicle #3 has been created
```
Whichever option we use, the idea is the same: the function object is copied into internal storage accessible to the new thread, and the new thread invokes the operator (). The Vehicle class can of course have data members and other member functions too, and this is one way of passing data to the thread function: pass it in as a constructor argument and store it as a data member:


**3- Lambda expression "Lambda" for short.**
With a Lambda you can easily create simple function objects.

### <u>Starting Threads with Lambdas</u>
A Lambda is just an object and, like other objects it may be copied, passed as a parameter, stored in a container, etc. The Lambda object has its own scope and lifetime which may, in some circumstances, be different to those objects it has ‘captured’. **Programers need to take special care when capturing local objects by reference because a Lambda’s lifetime may exceed the lifetime of its capture list:** **It must be ensured that the object to which the reference points is still in scope when the Lambda is called.** **This is especially important in multi-threading programs.**

**Example 3:**
The below code starts a thread and pass it a Lambda object to execute:

```
#include <iostream>
#include <thread>

int main()
{
    int id = 0; // Define an integer variable

    // starting a first thread (by reference)
    auto f0 = [&id]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "a) ID in Thread (call-by-reference) = " << id << std::endl;
    };
    std::thread t1(f0);

    // starting a second thread (by value)
    std::thread t2([id]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        std::cout << "b) ID in Thread (call-by-value) = " << id << std::endl;
    });

    // increment and print id in main
    ++id;
    std::cout << "c) ID in Main (call-by-value) = " << id << std::endl;

    // wait for threads before returning
    t1.join();
    t2.join();

    return 0;
}
```

**Example 3 o/p:**
```
c) ID in Main (call-by-value) = 1
b) ID in Thread (call-by-value) = 0
a) ID in Thread (call-by-reference) = 1
```
As you can see, the output in the main thread is generated first, at which point the variable ID has taken the value 1. Then, the call-by-value thread is executed with ID at a value of 0. Then, the call-by-reference thread is executed with ID at a value of 1. This illustrates the effect of passing a value by reference : when the data to which the reference refers changes before the thread is executed, those changes will be visible to the thread.