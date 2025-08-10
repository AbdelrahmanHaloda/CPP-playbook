


### Rule of Five

The Rule of Five is a C++ guideline for classes that manage resources like raw pointers, file handles, or network sockets. It states that if you need to write or delete any of the following five special member functions, you should probably deal with all five:

- Destructor
- Copy Constructor
- Copy Assignment Operator
- Move Constructor
- Move Assignment Operator

The reason is simple: if your class needs a custom destructor to release a resource (like calling delete[]), the default compiler-generated copy and move functions will be **incorrect**. They would perform a **shallow copy** of the pointer, leading to two objects pointing to the same memory. This results in double-free errors and undefined behavior.


#### summary:
Any change in **destructor** or **copy constructor** or **copy assigment operator** or **move constructor** or **move assigment operator** Must be followed by implementing the other 4.


```

#include <iostream>
#include <algorithm> // For std::copy
#include <utility>   // For std::swap and std::move

class MyMovableClass
{
private:
    int _size;
    int* _data;

public:
    
    // 1. Constructor
    MyMovableClass(int size) 
        : _size(size),
          _data(new int[size])
    {
        std::cout << "-> CONSTRUCTOR: Creating instance at " << this << " with " << _size << " elements.\n";
    }

    // 2. Destructor
    ~MyMovableClass()
    {
        std::cout << "-> DESTRUCTOR: Deleting instance at " << this << ".\n";
        delete[] _data;
    }

    // 3. Copy Constructor
    MyMovableClass(const MyMovableClass& source) 
        : _size(source._size), 
          _data(new int[source._size])
    {
        std::cout << "-> COPY CONSTRUCTOR: Copying from " << &source << " to " << this << ".\n";
        // copy the data from source
        std::copy(source._data, source._data + source._size, _data);
        /*
            for (int i = 0; i < _size; ++i) 
            {
                _data[i] = source._data[i];
            }
        */
    }

    // 4. Copy Assignment Operator (using copy-and-swap idiom)
    MyMovableClass& operator=(MyMovableClass source) noexcept
    {
        // 1. Check for self-assignment
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
          _data(nullptr) // Initialize to a valid state
    {
        std::cout << "-> MOVE CONSTRUCTOR: Moving from " << &source << " to " << this << ".\n";
        // "Steal" the resources from the source object
        _size = source._size;
        _data = source._data;

        // Leave the source object in a valid but empty state
        source._size = 0;
        source._data = nullptr;
    }

    // 6. Move Assignment Operator
    MyMovableClass& operator=(MyMovableClass&& source) noexcept
    {
        std::cout << "-> MOVE ASSIGNMENT: Moving from " << &source << " to " << this << ".\n";
        if (this != &source) // Prevent self-move-assignment
        {
            // Release our own resource
            delete[] _data;

            // "Steal" the resources from the source object
            _data = source._data;
            _size = source._size;

            // Leave the source object in a valid but empty state
            source._data = nullptr;
            source._size = 0;
        }
        return *this;
    }
};

```

---

### The Rule of Zero: A Modern Alternative
Before diving into the code, it's worth noting the modern C++ best practice: the Rule of Zero. This rule suggests you design your classes so they don't need to manage resources directly. Instead, use existing resource-managing classes from the standard library, like std::unique_ptr for pointers or std::vector for dynamic arrays.

When you follow the Rule of Zero, the compiler-generated special member functions work correctly, and you don't need to write any of the five yourself. It's simpler and safer. However, understanding the Rule of Five is still essential for learning C++ and for situations where you must manage a resource manually.

---