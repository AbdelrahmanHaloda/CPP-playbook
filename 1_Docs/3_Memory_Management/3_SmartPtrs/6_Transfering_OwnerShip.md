# Passing smart pointers to functions

Let us consider the following recommendation of the C++ guidelines on smart pointers:

**R. 30 :**

- Take smart pointers as parameters only to explicitly express lifetime semantics
The core idea behind this rule is the notion that functions that only manipulate objects without affecting its lifetime in any way should not be concerned with a particular kind of smart pointer.

- A function that does not manipulate the lifetime or ownership should use raw pointers or references instead.

- A function should take smart pointers as parameter only if it examines or manipulates the smart pointer itself.

- As we have seen, smart pointers are classes that provide several features such as counting the references of a `shared_ptr` or increasing them by making a copy. Also, data can be moved from one `unique_ptr` to another and thus transferring the ownership.

- A particular function should accept smart pointers only if it expects to do something of this sort.If a function just needs to operate on the underlying object without the need of using any smart pointer property, it should accept the objects via raw pointers or references instead.

**The following examples are pass-by-value types that lend the ownership of the underlying object:**

**1- Passing unique_ptr by value**
**Function Prototype:**

`void f(std::unique_ptr<MyObject> ptr)`

**Ownership Transfer:** When passing a `std::unique_ptr` by value, ownership of the resource is transferred to the function. The original `std::unique_ptr` becomes null after the transfer.

**Resource Management:** The function now owns the resource, and when the function exits, the `std::unique_ptr` goes out of scope, releasing the resource unless it is transferred again.

**Usage:** Used when you want a function to take full ownership and manage the lifetime of a resource.

**Example:**

```cpp
#include <memory>
#include <iostream>

void f(std::unique_ptr<int> ptr) {
    // Function owns the resource now
    std::cout << "Value inside f: " << *ptr << std::endl;
}

int main() {
    std::unique_ptr<int> p = std::make_unique<int>(42);
    
    // f(p); // This will not compile
    
    f(std::move(p)); // This works, transferring ownership to f
    
    // p is now null
    
    if (!p) {
        std::cout << "p is null after move." << std::endl;
    }
    return 0;
}
```

**2- Passing shared_ptr by value**
**Function Prototype:**

`void f(std::shared_ptr<MyObject> ptr)`

**Ownership Transfer:** When passing a `std::shared_ptr` by value, the reference count of the shared pointer is incremented. Both the caller and the function share ownership of the resource. The resource is only released when the last `std::shared_ptr` goes out of scope.

**Resource Management:** The function shares ownership of the resource with the caller. The resource is managed through reference counting, ensuring it is only released when no more std::shared_ptrs own it.

**Usage:** Used when you want multiple parts of the program to share ownership of a resource.

```cpp
void f(std::shared_ptr<int> ptr) {
    // Function shares ownership of the resource
    std::cout << "Value inside f: " << *ptr << std::endl;
}

int main() {
    std::shared_ptr<int> p = std::make_shared<int>(42);
    f(p); // Shared ownership, reference count increases
    std::cout << "Reference count after f: " << p.use_count() << std::endl;
    return 0;
}
```

**3- Passing weak_ptr by value**
**Function Prototype:**

`void f(std::weak_ptr<MyObject> ptr)`

**Ownership Transfer:** `std::weak_ptr` does not own the resource. Instead, it holds a non-owning reference to a resource managed by a `std::shared_ptr`. Passing a `std::weak_ptr` does not change the ownership or the reference count of the resource.

**Resource Management:** The resource is managed by the `std::shared_ptr` that the `std::weak_ptr` points to. The `std::weak_ptr` can be used to create a temporary `std::shared_ptr` if the resource is still available.

**Usage:** Used to avoid circular references and prevent resource leaks in situations where a non-owning reference is needed.

```cpp
void f(std::weak_ptr<int> weakPtr) {
    if (auto sharedPtr = weakPtr.lock()) 
    {
        // Temporarily shares ownership of the resource
        std::cout << "Value inside f: " << *sharedPtr << std::endl;
    } else 
    {
        std::cout << "Resource no longer available" << std::endl;
    }
}

int main() {
    std::shared_ptr<int> p = std::make_shared<int>(42);
    std::weak_ptr<int> w = p;
    f(w); // Non-owning reference, no change in reference count
    return 0;
}
```

**Summary:**

**Ownership Semantics:**

- **When passing `std::unique_ptr` by value**, ownership is indeed transferred to the function. The original pointer in the caller is left empty (null) after the transfer.

- **When passing `std::shared_ptr` by value**, ownership is shared between the caller and the function. The reference count is incremented, and both the caller and the function can manage the resource.

- **When passing `std::weak_ptr` by value**, there is no transfer of ownership because `std::weak_ptr` is a non-owning reference. It holds a reference to the resource managed by a std::shared_ptr without affecting the reference count.

**Private Copy:**

- **For `std::unique_ptr`**, passing by value does not create a copy in the traditional sense, as `std::unique_ptr` cannot be copied. Instead, it is moved, transferring ownership.

- **For `std::shared_ptr`**, passing by value does create a new shared pointer that increases the reference count.

- **For `std::weak_ptr`**, passing by value creates a new `std::weak_ptr`, which is a lightweight operation and does not affect the resource management.

---

**R.33:**
Take a `unique_ptr&` parameter to express that a function reseats the widget

and

**R.35:**
Take a `shared_ptr&` parameter to express that a function might reseat the shared pointer

**Both rules recommend passing-by-reference**, when the function is supposed to modify the ownership of an existing smart pointer and not a copy. We pass a non-const reference to a **unique_ptr** to a function if it might modify it in any way, including deletion and reassignment to a different resource.

Passing a **unique_ptr** as const is not useful as the function will not be able to do anything with it: Unique pointers are all about proprietary ownership and as soon as the pointer is passed, the function will assume ownership. But without the right to modify the pointer, the options are very limited.

A **shared_ptr** can either be passed as const or non-const reference. The const should be used when you want to express that the function will only read from the pointer or it might create a local copy and share ownership.

Lastly, we will take a look at passing raw pointers and references. The general rule of thumb is that we can use a simple raw pointer (which can be null) or a plain reference (which can not be null), when the function we are passing will only inspect the managed object without modifying the smart pointer. The internal (raw) pointer to the object can be retrieved using the `get()` member function. Also, by providing access to the raw pointer, you can use the smart pointer to manage memory in your own code and pass the raw pointer to code that does not support smart pointers.

When using raw pointers retrieved from the `get()` function, you should take special care not to delete them or to create new smart pointers from them. If you did so, the ownership rules applying to the resource would be severely violated. When passing a raw pointer to a function or when returning it (see next section), raw pointers should always be considered as

---

**Returning smart pointers from functions**
With return values, the same logic that we have used for passing smart pointers to functions applies: Return a smart pointer, both unique or shared, if the caller needs to manipulate or access the pointer properties. In case the caller just needs the underlying object, a raw pointer should be returned.

**Smart pointers should always be returned by value.** This is not only simpler but also has the following advantages:

- The overhead usually associated with return-by-value due to the expensive copying process is significantly mitigated by the built-in move semantics of smart pointers. They only hold a reference to the managed object, which is quickly switched from destination to source during the move process.

- Since C++17, the compiler used Return Value Optimization (RVO) to avoid the copy usually associated with return-by-value. This technique, together with copy-elision, is able to optimize even move semantics and smart pointers (not in call cases though, they are still an essential part of modern C++).

- When returning a **shared_ptr** by value, the internal reference counter is guaranteed to be properly incremented. This is not the case when returning by pointer or by reference.

- The topic of smart pointers is a complex one. In this course, we have covered many basics and some of the more advanced concepts. However, there are many more aspects to consider and features to use when integrating smart pointers into your code. The full set of smart pointer rules in the C++ guidelines is a good start to dig deeper into one of the most powerful features of modern C++.

---

## Best-Practices for Passing Smart Pointers

This sections contains a condensed summary of when (and when not) to use smart pointers and how to properly pass them between functions. This section is intended as a guide for your future use of this important feature in modern C++ and will hopefully encourage you not to ditch raw pointers altogether but instead to think about where your code could benefit from smart pointers - and when it would most probably not.

The following list contains all the variations (omitting const) of passing an object to a function:

```cpp
void f( object* );  // (a)
void f( object& ); // (b)
void f( unique_ptr<object> ); // (c)
void f( unique_ptr<object>& ); // (d)
void f( shared_ptr<object> ); // (e)
void f( shared_ptr<object>& ); // (f)
```

The Preferred Way
The preferred way of to pass object parameters is by using a) or b) :

```cpp
void f( object* );             
void f( object& );            
```

In doing so, we do not have to worry about the lifetime policy a caller might have implemented. Using a specific smart pointer in a case where we only want to observe an object or manipulate a member might be overly restrictive.

With the non-owning raw pointer * or the reference & we can observe an object from which we can assume that its lifetime will exceed the lifetime of the function parameter. In concurrency however, this might not be the case, but for linear code we can safely assume this.

To decide wether a * or & is more appropriate, you should think about wether you need to express that there is no object. This can only be done with pointers by passing e.g. nullptr. In most other cases, you should use a reference instead.

**The Object Sink**
The preferred way of passing an object to a function so that the function takes ownership of the object (or „consumes“ it) is by using method c) from the above list:

`void f( unique_ptr<object> );`

In this case, we are passing a unique pointer by value from caller to function, which then takes ownership of the the pointer and the underlying object. This is only possible using move semantics as there may be only a single reference to the object managed by the unique pointer.

After the object has been passed in this way, the caller will have an invalid unique pointer and the function to which the object now belongs may destroy it or move it somewhere else.

Using const with this particular call does not make sense as it models an ownership transfer so the source will be definitely modified.

**In And Out Again 1**
In some cases, we want to modify a unique pointer (not necessarily the underlying object) and re-use it in the context of the caller. In this case, method d) from the above list might be most suitable:

`void f( unique_ptr<object>& );`

Using this call structure, the function states that it might modify the smart pointer, e.g. by redirecting it to another object. It is not recommended to use it for accepting an object only because we should avoid restricting ourselves unnecessarily to a particular object lifetime strategy on the caller side.

Using const with this call structure is not recommendable as we would not be able to modify the unique_ptr in this case. In case you want to modify the underlying object, use method a) instead.

**Sharing Object Ownership**
In the last examples, we have looked at strategies involving unique ownership. In this example, we want to express that a function will store and share ownership of an object on the heap. This can be achieved by using method e) from the list above:

`void f( shared_ptr<object> )`

In this example, we are making a copy of the shared pointer passed to the function. In doing so, the internal reference counter within all shared pointers referring to the same heap object is incremented by one.

This strategy can be recommended for cases where the function needs to retain a copy of the shared_ptr and thus share ownership of the object. This is of interest when we need access to smart pointer functions such as the reference count or we must make sure that the object to which the shared pointer refers is not prematurely deallocated (which might happen in concurrent programming).

If the local scope of the function is not the final destination, a shared pointer can also be moved, which does not increase the reference count and is thus more effective.

A disadvantage of using a shared_ptr as a function argument is that the function will be limited to using only objects that are managed by shared pointers - which limits flexibility and reusability of the code.

**In And Out Again 2**
As with unique pointers, the need to modify shared pointers and re-use them in the context of the caller might arise. In this case, method f) might be the right choice:

`void f( shared_ptr<object>& );`

This particular way of passing a shared pointer expresses that the function f will modify the pointer itself. As with method e), we will be limiting the usability of the function to cases where the object is managed by a shared_ptr and nothing else.

**Last Words**
The topic of smart pointers is a complex one. In this course, we have covered many basics and some of the more advanced concepts. However, for some cases there are more aspects to consider and features to use when integrating smart pointers into your code. [The full set of smart pointer rules](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#rsmart-smart-pointers) in the C++ guidelines is a good start to dig deeper into one of the most powerful features of modern C++.

---
