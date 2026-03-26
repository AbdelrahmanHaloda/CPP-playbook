
# Rule of Five

The Rule of Five is a C++ guideline for classes that manage resources like raw pointers, file handles, or network sockets. It states that if you need to write or delete any of the following five special member functions, you should probably deal with all five:

- Destructor
- Copy Constructor
- Copy Assignment Operator
- Move Constructor
- Move Assignment Operator

The reason is simple: if your class needs a custom destructor to release a resource (like calling delete[]), the default compiler-generated copy and move functions will be **incorrect**. They would perform a **shallow copy** of the pointer, leading to two objects pointing to the same memory. This results in double-free errors and undefined behavior.

## summary

Any change in **destructor** or **copy constructor** or **copy assigment operator** or **move constructor** or **move assigment operator** Must be followed by implementing the other 4.

```c++
#include <iostream>
#include <algorithm> // For std::copy
#include <utility>   // For std::swap and std::move

class MyMovableClass
{
private:
    int _size;
    int* _pData;

public:
    
    // 1. Constructor
    MyMovableClass(int size) 
        : _size(size),
          _pData(new int[size])
    {
        std::cout << "-> CONSTRUCTOR: Creating instance at " << this << " with " << _size << " elements.\n";
    }

    // 2. Destructor
    ~MyMovableClass()
    {
        std::cout << "-> DESTRUCTOR: Deleting instance at " << this << ".\n";
        delete[] _pData;
    }

    // 3. Copy Constructor
    MyMovableClass(const MyMovableClass& source) 
        : _size(source._size), 
          _pData(new int[source._size])
    {
        std::cout << "-> COPY CONSTRUCTOR: Copying from " << &source << " to " << this << ".\n";
        // copy the data from source
        std::copy(source._pData, source._pData + source._size, _pData);
        /*
            for (int i = 0; i < _size; ++i) 
            {
                _pData[i] = source._pData[i];
            }
        */
    }

    // 4. Copy Assignment Operator (using copy-and-swap idiom)
    MyMovableClass& operator=(const MyMovableClass source) noexcept
    {
        // Check for self-assignment
        // This check is necessary only for copy-assignment operator because its the case where you copy an object 
        // to an existing // object (both objects already exist so you might do something like this objectA = objectA)
        // Unlike copy constructor which is triggered when you instantiate a new object from an existing object so no risk since
        // this operation requires new memory allocation.
        if (this == &source) {
            return *this;
        }

        // 2. Delete existing data
        delete _pData; // PROBLEM: What if the next line fails?

        // 3. Allocate new memory and copy
        _pData = new int; // This could fail and throw an exception! so it is better to use copy and swap technique
        *_pData = *source._pData;

        return *this;
    }

    // 5. Move Constructor
    MyMovableClass(MyMovableClass&& source) noexcept
        : _size(0), 
          _pData(nullptr) // Initialize to a valid state
    {
        std::cout << "-> MOVE CONSTRUCTOR: Moving from " << &source << " to " << this << ".\n";
        // "Steal" the resources from the source object
        _size = source._size;
        _pData = source._pData;

        // Leave the source object in a valid but empty state
        source._size = 0;
        source._pData = nullptr;
    }

    // 6. Move Assignment Operator
    MyMovableClass& operator=(MyMovableClass&& source) noexcept
    {
        std::cout << "-> MOVE ASSIGNMENT: Moving from " << &source << " to " << this << ".\n";
        if (this != &source) // Prevent self-move-assignment
        {
            // Release our own resource
            delete[] _pData;

            // "Steal" the resources from the source object
            _pData = source._pData;
            _size = source._size;

            // Leave the source object in a valid but empty state
            source._pData = nullptr;
            source._size = 0;
        }
        return *this;
    }
};

```

---

## why copy constructor has this signature ?

> MyMovableClass(const MyMovableClass& source)

1) It must take the source by reference
If it took it by value, creating the parameter would itself require calling the copy constructor again, causing infinite recursion.

2) It is const so it can copy from const objects
A copy operation should work even when the source object is const. A non-const reference (T&) cannot bind to a const object.

3) It is const because copying should not modify the source
The source is only read to create an independent copy, so the type should express that intent.

4) `const T&` can also bind to rvalues
This makes the copy constructor broadly usable. In modern C++, rvalues are usually handled by the move constructor (T&&) when available, but if moving is unavailable, the copy constructor can still accept them.
