# The worst problems with new and delete:

- **Proper pairing of new and delete :** Every dynamically allocated object that is created with new must be followed by a manual deallocation at a "proper" place in the program. If the programer forgets to call delete (which can happen very quickly) or if it is done at an "inappropriate" position, memory leaks will occur which might clog up a large portion of memory.

- **Correct operator pairing :** C++ offers a variety of new/delete operators, especially when dealing with arrays on the heap. A dynamically allocated array initialized with new[] may only be deleted with the operator delete[]. If the wrong operator is used, program behavior will be undefined - which is to be avoided at all cost in C++.

- **Memory ownership :** If a third-party function returns a pointer to a data structure, the only way of knowing who will be responsible for resource deallocation is by looking into either the code or the documentation. If both are not available (as is often the case), there is no way to infer the ownership from the return type. As an example, in the final project of this course, we will use the graphical library wxWidgets to create the user interface of a chatbot application. In wxWidgets, the programmer can create child windows and control elements on the heap using new, but the framework will take care of deletion altogether. If for some reason the programmer does not know this, he or she might call delete and thus interfere with the inner workings of the wxWidgets library.

---

## The benefits of smart pointers

**To put it briefly:**
Smart pointers were introduced in C++ to solve the above mentioned problems by providing a degree of automatic memory management: When a smart pointer is no longer needed (which is the case as soon as it goes out of scope), the memory to which it points is automatically deallocated. When contrasted with smart pointers, the conventional pointers we have seen so far are often termed "raw pointers".

In essence, smart pointers are classes that are wrapped around raw pointers. By overloading the -> and * operators, smart pointer objects make sure that the memory to which their internal raw pointer refers to is properly deallocated. This makes it possible to use smart pointers with the same syntax as raw pointers. As soon as a smart pointer goes out of scope, its destructor is called and the block of memory to which the internal raw pointer refers is properly deallocated. This technique of wrapping a management class around a resource has been conceived by Bjarne Stroustroup and is called **Resource Acquisition Is Initialization (RAII)**. Before we continue with smart pointers and their usage let us take a close look at this powerful concept.

---

## Smart pointer overview

Since C++11, the standard library includes smart pointers, which help to ensure that programs are free of memory leaks while also remaining exception-safe. With smart pointers, resource acquisition occurs at the same time that the object is initialized (when instantiated with make_shared or make_unique), so that all resources for the object are created and initialized in a single line of code.

In modern C++, raw pointers managed with new and delete should only be used in small blocks of code with limited scope, where performance is critical (such as with placement new) and ownership rights of the memory resource are clear. We will look at some guidelines on where to use which pointer later.

C++11 has introduced three types of smart pointers, which are defined in the header of the standard library:

- **The unique pointer std::unique_ptr** is a smart pointer which exclusively owns a dynamically allocated resource on the heap. There must not be a second unique pointer to the same resource.

- **The shared pointer std::shared_ptr** points to a heap resource but does not explicitly own it. There may even be several shared pointers to the same resource, each of which will increase an internal reference count. As soon as this count reaches zero, the resource will automatically be deallocated.

- **The weak pointer std::weak_ptr** behaves similar to the shared pointer but does not increase the reference counter.

Prior to C++11, there was a concept called std::auto_ptr, which tried to realize a similar idea. However, this concept can now be safely considered as deprecated and should not be used anymore.

---

## Converting between smart pointers

The example below illustrates how to convert between the different pointer types.

In (1), a conversion from unique pointer to shared pointer is performed. You can see that this can be achieved by using `std::move`, which calls the move assignment operator on sharedPtr1 and steals the resource from uniquePtr while at the same time invalidating its resource handle on the heap-allocated integer.

In (2), you can see how to convert from weak to shared pointer. Imagine that you have been passed a weak pointer to a memory object which you want to work on. To avoid invalid memory access, you want to make sure that the object will not be deallocated before your work on it has been finished. To do this, you can convert a weak pointer to a shared pointer by calling the `lock()` function on the weak pointer.

In (3), a raw pointer is extracted from a shared pointer. However, this operation does not decrease the reference count within sharedPtr2. This means that calling delete on rawPtr in the last line before main returns will generate a runtime error as a resource is trying to be deleted which is managed by sharedPtr2 and has already been removed. The output of the program when compiled with g++ thus is:

```cpp
malloc: *** error for object 0x1003001f0: pointer being freed was not allocated.
```

```cpp
#include <iostream>
#include <memory>

int main()
{
    // construct a unique pointer
    std::unique_ptr<int> uniquePtr(new int);
    
    // (1) shared pointer from unique pointer
    std::shared_ptr<int> sharedPtr1 = std::move(uniquePtr);

    // (2) shared pointer from weak pointer
    std::weak_ptr<int> weakPtr(sharedPtr1);
    std::shared_ptr<int> sharedPtr2 = weakPtr.lock(); // convert weak ptr to shared ptr

    // (3) raw pointer from shared (or unique) pointer   
    int *rawPtr = sharedPtr2.get();
    delete rawPtr;

    return 0;
}
```

Note that there are no options for converting away from a shared pointer. Once you have created a shared pointer, you must stick to it (or a copy of it) for the remainder of your program.

---
