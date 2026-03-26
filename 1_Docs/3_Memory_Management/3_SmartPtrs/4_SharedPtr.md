# The Shared Pointer

Just as the unique pointer, a shared pointer owns the resource it points to. The main difference between the two smart pointers is that shared pointers keep a reference counter on how many of them point to the same memory resource. Each time a shared pointer goes out of scope, the counter is decreased. When it reaches zero (i.e. when the last shared pointer to the resource is about to vanish). the memory is properly deallocated. This smart pointer type is useful for cases where you require access to a memory location on the heap in multiple parts of your program and you want to make sure that whoever owns a shared pointer to the memory can rely on the fact that it will be accessible throughout the lifetime of that pointer.

The following diagram illustrates the basic idea of a shared pointer:
[Diagram](/Users/abdelrahmanhaloda/AHossam/REPOS/CPP-playbook/1_Docs/3_Memory_Management/Images/SharedPtr.png)

---

## shared_ptr mthodes

1- `.get()`

returns the raw pointer

`*(shared.get())` is equivalent to `*shared_ptr`

2- `.use_count()`
returns the number of pointers shared to the same object.

3- `.reset()`
A method that modifies the `std::shared_ptr` so that it either points to a new object or is empty. This operation can involve releasing ownership of the currently managed object, potentially destroying it if no other `std::shared_ptr` instances share ownership.

Example: Resetting to an Empty std::shared_ptr

```cpp
#include <iostream>
#include <memory>

class MyClass {
public:
    MyClass() {
        std::cout << "MyClass constructor called." << std::endl;
    }
    ~MyClass() {
        std::cout << "MyClass destructor called." << std::endl;
    }
};

int main() {
    std::shared_ptr<MyClass> sharedPtr = std::make_shared<MyClass>();

    std::cout << "Resetting sharedPtr to empty." << std::endl;
    sharedPtr.reset();  // Releases ownership of the object

    if (!sharedPtr) {
        std::cout << "sharedPtr is now empty." << std::endl;
    }

    return 0;
}
```

Example: Resetting to a New Object

```cpp
#include <iostream>
#include <memory>

class MyClass {
public:
    MyClass(int value) : value(value) {
        std::cout << "MyClass constructor called with value: " << value << std::endl;
    }
    ~MyClass() {
        std::cout << "MyClass destructor called with value: " << value << std::endl;
    }

private:
    int value;
};

int main() {
    std::shared_ptr<MyClass> sharedPtr = std::make_shared<MyClass>(100);

    std::cout << "Resetting sharedPtr to a new object." << std::endl;
    sharedPtr.reset(new MyClass(200));  // Releases ownership of the original object and points to a new one

    return 0;
}
```

---
