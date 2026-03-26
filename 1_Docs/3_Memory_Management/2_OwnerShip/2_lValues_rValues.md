# What are lvalues and rvalues?

A good grasp of lvalues and rvalues in C++ is essential for understanding the more advanced concepts of rvalue references and motion semantics.

Every expression in C++ has a type and belongs to a value category. When objects are created, copied or moved during the evaluation of an expression, the compiler uses these value expressions to decide which method to call or which operator to use.

Prior to C++11, there were only two value categories, now there are as many as five of them:

To keep it short, we do not want to go into all categories, but limit ourselves to lvalues and prvalues:

**Lvalues** have an address that can be accessed. They are expressions whose evaluation by the compiler determines the identity of objects or functions.

**Prvalues** do not have an address that is accessible directly. They are temporary expressions used to initialize objects or compute the value of the operand of an operator.

For the sake of simplicity and for compliance with many tutorials, videos and books about the topic, let us refer to prvalues as rvalues from here on.

The two characters l and r are originally derived from the perspective of the assignment operator =, which always expects a rvalue on the right, and which it assigns to a lvalue on the left. In this case, the l stands for left and r for right:

`int i = 42;  // lvalue = rvalue;`

With many other operators, however, this right-left view is not entirely correct. In more general terms, an lvalue is an entity that points to a specific memory location. An rvalue is usually a short-lived object, which is only needed in a narrow local scope. To simplify things a little, one could think of lvalues as named containers for rvalues.

In the example above, the value 42 is an rvalue. It does not have a specific memory address which we know about. The rvalue is assigned to a variable i with a specific memory location known to us, which is what makes it an lvalue in this example.

Using the address operator & we can generate an lvalue from an rvalue and assign it to another lvalue:
`int *j = &i;`

[How to crack the confusing world of lvalues and rvalues in C++? It is easy!](https://medium.com/@dhaneshvb/how-to-crack-the-confusing-world-of-lvalues-and-rvalues-in-c-it-is-easy-61c32ced51ce)

[Lvalues and Rvalues (C++)](https://learn.microsoft.com/en-us/cpp/cpp/lvalues-and-rvalues-visual-cpp?view=msvc-160)

---

## What is lvalue reference ?

- lValue reference is as it sounds exactly, `int& x`;

- lValue reference can't be bind to an rvalue **BUT** a lvalue reference to a const can.

**Example 1**

```c++
void func(int& x)
{}
int main()
{
    int x;
    func(x);    // works fine
    func(42);   // Error, Can't bind lvalue reference to a rvalue
}
```

**Example 2**

```c++
void func(const int& x)
{}
int main()
{
    int x;
    func(x);    // works fine
    func(42);   // works fine
}
```

**So, from the above example we can see why the copy constructor and copy assigment operator takes a const lvalue reference so it can handle lvalue and rvalue passed to them !**

---

### what is rvalue reference and why it is introduced in c++11?

An rvalue reference (&&) is a feature introduced in C++11 that allows the compiler to reference temporary, short-lived objects called rvalues. Its primary purpose is to enable move semantics, a powerful optimization that avoids expensive resource copying by instead "moving" or "stealing" resources from temporary objects that are about to be destroyed anyway.

**`int &&X; //rvalue reference`**

#### The Problem Before C++11: Expensive Copying
Imagine you have a class that manages a large block of memory, like a custom vector or a string.

```c++
// A heavy object that allocates a lot of memory
class HeavyObject {
public:
    HeavyObject() {
        // Allocate a large chunk of memory
        data = new int[1000]; 
        std::cout << "Constructor: Allocated memory.\n";
    }

    // COPY CONSTRUCTOR (Expensive)
    HeavyObject(const HeavyObject& other) {
        // Allocate a NEW large chunk of memory
        data = new int[1000];
        // Copy every single element from the source
        std::copy(other.data, other.data + 1000, data);
        std::cout << "COPY: Cloned a heavy object.\n";
    }

    ~HeavyObject() {
        delete[] data;
        std::cout << "Destructor: Freed memory.\n";
    }

private:
    int* data;
};

HeavyObject createObject() {
    return HeavyObject(); // Creates an object and returns it
}

int main() {
    HeavyObject myObject = createObject(); // The temporary object from createObject() is COPIED
}
```

The temporary HeavyObject created inside the function is about to be destroyed, yet we still perform a full, expensive copy of its 1000 integers. This is like building a brand-new, identical house instead of just handing over the keys to the old one.


#### The C++11 Solution: Move Semantics with Rvalue References
C++11 introduced the move constructor, which takes an rvalue reference (&&). This special reference can bind to temporary objects (rvalues), allowing us to safely "steal" their resources.

Here is the HeavyObject class updated with a move constructor.

```c++
#include <iostream>
#include <algorithm> // for std::copy
#include <utility>   // for std::move

class HeavyObject {
public:
    // Default constructor
    HeavyObject() {
        data = new int[1000];
        std::cout << "Constructor: Allocated memory.\n";
    }

    // 1. COPY CONSTRUCTOR (The expensive way)
    HeavyObject(const HeavyObject& other) {
        std::cout << "COPY: Cloning a heavy object (expensive!).\n";
        data = new int[1000];
        std::copy(other.data, other.data + 1000, data); // Deep copy
    }

    // 2. MOVE CONSTRUCTOR (The cheap way, using rvalue reference)
    HeavyObject(HeavyObject&& other) noexcept { // Takes an rvalue reference &&
        std::cout << "MOVE: Stealing resources from a temporary object (cheap!).\n";
        
        // Step 1: Perform a shallow copy of the memory handler (the pointer).
        this->data = other.data;
        
        // Step 2: Invalidate the original object's handler to prevent double-freeing memory.
        other.data = nullptr; 
    }

    // Destructor
    ~HeavyObject() {
        if (data != nullptr) {
            std::cout << "Destructor: Freed memory.\n";
        } else {
            std::cout << "Destructor: Nothing to free (was moved from).\n";
        }
        delete[] data;
    }

private:
    int* data;
};

// A function that returns a HeavyObject by value
HeavyObject createObject() {
    std::cout << "--- Starting createObject() ---\n";
    HeavyObject temp;
    std::cout << "--- Finishing createObject() ---\n";
    return temp; // This return creates a temporary object (an rvalue)
}

int main() {
    std::cout << "## Scenario 1: Automatic Move ##\n";
    // The compiler sees we are initializing from a temporary (rvalue)
    // from createObject() and automatically calls the MOVE constructor.
    HeavyObject obj1 = createObject();

    std::cout << "\n## Scenario 2: Explicit Move ##\n";
    HeavyObject obj2; // A regular object (an lvalue)
    // We can't do: HeavyObject obj3 = obj2; // This would COPY
    
    // We must explicitly cast obj2 to an rvalue to trigger the move.
    // std::move signals: "You can treat obj2 as a temporary from now on."
    HeavyObject obj3 = std::move(obj2);

    std::cout << "\n--- End of main ---\n";
    return 0;
}
```

**output:**

```shell
## Scenario 1: Automatic Move ##
--- Starting createObject() ---
Constructor: Allocated memory.
--- Finishing createObject() ---
MOVE: Stealing resources from a temporary object (cheap!).
Destructor: Nothing to free (was moved from).

## Scenario 2: Explicit Move ##
Constructor: Allocated memory.
MOVE: Stealing resources from a temporary object (cheap!).

--- End of main ---
Destructor: Freed memory.
Destructor: Nothing to free (was moved from).
Destructor: Freed memory.
```

### Key Takeaways from the Example

**Automatic Move:**
 In Scenario 1, the compiler is smart enough to see that the object being returned from createObject is a temporary. It automatically chooses the move constructor over the copy constructor, avoiding the expensive allocation and deep copy.

**Explicit Move**
 with std::move: In Scenario 2, obj2 is a normal variable with a name (an lvalue). We can't directly bind an rvalue reference to it. std::move is used to cast the lvalue obj2 into an rvalue, essentially giving the compiler permission to "steal" from it, thus invoking the move constructor for obj3. After the move, obj2 is left in a valid but unspecified state (we made its pointer nullptr).

**Why const T& Wasn't Enough:**
 You can't use a regular const lvalue reference (const HeavyObject&) for moving because it's const. It forbids modification, making it impossible to invalidate the original object's pointer (other.data = nullptr;), which is a critical step to prevent the same memory from being deleted twice. The && rvalue reference is non-const, specifically allowing this modification.

- rvalue reference can't be bind to lvalue. (that's why move semantics is introduced to resolve this problem).

- rvalue reference is introduced in c++ to enhance the performance of code in the situation of moving resources instead of copying it.

- prior to c++11 when we want to move a resource, we had to copy it and this is copy could be an expensive operation if the resource is a heavy object and by heavy i mean an object that allocates memory on the heap or has a huge size.

- so, basically rvalue reference is used in the move constructor to do 2 things:
   1- perform a shallow copy of the memory handler.
   2- perform ownership management by copying the memory handler from the orignal source to it then invalidating the memory handler in the orignal.
This can't be done using lvalue reference since lvalue references can't bind to rvalue except if they are const which will make the step number 2 impossible (invalidating the memory handler in the orignal source).

---

## Binding rules for lvalues and rvalues in C++.

**Think of it like this:**

- An **lvalue** is something with a persistent address, like a house. 🏡

- An **rvalue** is something temporary, like a number in a calculation or a flyer handed to you. 📄

### Lvalue Reference (T&)

An lvalue reference is a standard reference that acts as an alias for an existing object.

**Can bind to:** lvalues only.

**Cannot bind to:** rvalues. This prevents you from getting a reference to a temporary object that will disappear, leaving you with a dangling reference.

```c++

int x = 10; // x is an lvalue

// --- Valid ---
int& ref_to_lvalue = x; // OK: lvalue reference binds to lvalue

// --- Invalid ---
// int& ref_to_rvalue = 10; // ERROR: Cannot bind lvalue reference to rvalue
```

### Rvalue Reference (T&&)

An rvalue reference is specifically designed to bind to temporary objects. It's the key mechanism that enables move semantics.

**Can bind to:** rvalues only.

**Cannot bind to:** lvalues.

```c++
int x = 10; // x is an lvalue

// --- Valid ---
int&& ref_to_rvalue = 10; // OK: rvalue reference binds to rvalue
int&& moved_ref = std::move(x); // OK: std::move casts x to an rvalue

// --- Invalid ---
// int&& ref_to_lvalue = x; // ERROR: Cannot bind rvalue reference to lvalue
```

### Const Lvalue Reference (const T&)

This is the most flexible type of reference. Because it's const, it promises not to change the object it refers to, making it safe to bind to anything.

**Can bind to:** both lvalues and rvalues.

This is why it's the preferred way to pass objects to functions when you only need read-only access and want to avoid making a copy.

```c++
int x = 10; // x is an lvalue

// --- Valid ---
const int& ref_to_lvalue = x;   // OK: const lvalue reference binds to lvalue
const int& ref_to_rvalue = 10;  // OK: const lvalue reference binds to rvalue
```

---

## Summary Table

Here's a table summarizing which reference type can bind to which value category.

|               Reference Type.              | Lvalue (`int x`) | Rvalue (`10`) |
| :----------------------------------------: | :--------------: | :-----------: |
| **Lvalue Reference (`int&`)**              | ✅ Yes           | ❌ No          |
| **Rvalue Reference (`int&&`)**             | ❌ No            | ✅ Yes         |
| **Const Lvalue Reference (`const int&`)**  | ✅ Yes           | ✅ Yes         |
| **Const Rvalue Reference** (`const int&&`) | ❌ No            | ✅ Yes         |

**Note:**
A const rvalue reference (const T&&) is a reference that binds only to rvalues but prevents you from modifying them.

In practice, it is almost never used because it creates a logical contradiction. The entire point of an rvalue reference (T&&) is to bind to a temporary object so you can safely modify it (i.e., "steal" its resources via move semantics). Adding const prevents this modification, defeating the purpose.

---
