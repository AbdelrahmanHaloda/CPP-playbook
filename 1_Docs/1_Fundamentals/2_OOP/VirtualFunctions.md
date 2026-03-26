# Virtual Functions

Virtual functions are a polymorphic feature. These functions are declared (and possibly defined) in a base class, and can be overridden by derived classes.

This approach declares an interface at the base level, but delegates the implementation of the interface to the derived classes.

A pure virtual function is a virtual function that the base class declares but does not define.

A pure virtual function has the side effect of making its class abstract. This means that the class cannot be instantiated. Instead, only classes that derive from the abstract class and override the pure virtual function can be instantiated.

## Why do we need virtual functions?

### Problem (static binding)

```cpp
class Base {
public:
    void foo() { std::cout << "Base\n"; }
};

class Derived : public Base {
public:
    void foo() { std::cout << "Derived\n"; }
};

Base* b = new Derived();
b->foo(); // ❌ calls Base::foo
```

👉 Reason:
- Binding happens at **compile-time**
- Compiler only sees `Base*`

---

## Solution: `virtual`

```cpp
class Base {
public:
    virtual void foo() { std::cout << "Base\n"; }
};
```

```cpp
b->foo(); // ✅ calls Derived::foo
```

👉 Now resolved at **runtime** → dynamic dispatch

---

## Core idea

> Virtual functions enable **runtime polymorphism**

The function call is resolved at runtime based on the **actual object type**, not the pointer type.

---

## How it works (vtable concept)

Each class with virtual functions has:

- a **vtable** (table of function pointers)
- each object has a hidden pointer → **vptr**

### Call flow

```cpp
b->foo();
```

Internally:

```
b → vptr → vtable → Derived::foo
```

---

## vtable intuition

```
Base vtable:
[ foo → Base::foo ]

Derived vtable:
[ foo → Derived::foo ]
```

Each object points to the vtable of its actual type.

---

## Example (real usage)

```cpp
#include <iostream>

class Animal {
public:
    virtual void speak() const {
        std::cout << "Animal sound\n";
    }

    virtual ~Animal() = default;
};

class Dog : public Animal {
public:
    void speak() const override {
        std::cout << "Woof\n";
    }
};

int main() {
    Animal* animal = new Dog{};
    animal->speak(); // Dog::speak()
    delete animal;
}
```

---

## `override` keyword (IMPORTANT)

```cpp
class Derived : public Base {
public:
    void foo() override;
};
```

### Why?
- Ensures correct overriding
- Prevents signature mismatch bugs

### What happens WITHOUT `override`?

C++ will still compile, but it does **not guarantee** you actually overrode the base function.

Example:

```cpp
class Base {
public:
    virtual void foo(int x) {}
};

class Derived : public Base {
public:
    void foo(double x) {} // ❌ NOT overriding
};
```

👉 This compiles, but:
- `Derived::foo(double)` is a **new function**, not an override
- Base version is still used in polymorphism

```cpp
Base* b = new Derived();
b->foo(10); // calls Base::foo(int) ❌
```

---

### What `override` guarantees

```cpp
class Derived : public Base {
public:
    void foo(double x) override; // ❌ compile-time error
};
```

👉 Compiler error:
- "no function to override"

---

### Common mistakes it catches

#### 1. Wrong parameter type
```cpp
void foo(double x) override; // ❌
```

#### 2. Missing const
```cpp
class Base {
public:
    virtual void foo() const;
};

class Derived : public Base {
public:
    void foo() override; // ❌ missing const
};
```

#### 3. Different qualifiers
```cpp
virtual void foo() &;
void foo() && override; // ❌ mismatch
```

---

### Mental model

- Without `override` → "maybe override, maybe new function"
- With `override` → "must override or fail"

---

### Takeaway

> `override` turns silent bugs into compile-time errors.

---

## Pure virtual functions

```cpp
class Shape {
public:
    virtual double area() const = 0;
};
```

- No implementation in base
- Makes class **abstract**

---

## Abstract class

```cpp
Shape s; // ❌ not allowed
```

Derived classes must implement all pure virtual functions.

---

## 🔴 Virtual Destructor (CRITICAL)

### Problem

```cpp
class Base {
public:
    ~Base() {}
};

class Derived : public Base {
public:
    ~Derived() { std::cout << "cleanup\n"; }
};

Base* b = new Derived();
delete b; // ❌ only Base destructor called
```

👉 Result:
- Derived destructor not called
- Resource leak
- Undefined behavior

---

### Solution

```cpp
class Base {
public:
    virtual ~Base() = default;
};
```

👉 Now:
- Derived → Base destruction (correct order)

---

### Rule

> If a class is used polymorphically → destructor MUST be virtual

---

## Object slicing (VERY IMPORTANT)

### Definition

> Object slicing happens when a **derived object is copied into a base object by value**, causing the derived part to be lost.

---

### Example

```cpp
class Base {
public:
    int x = 1;
};

class Derived : public Base {
public:
    int y = 2;
};

Derived d;
Base b = d; // ❌ slicing
```

---

### What actually happens

- `b` is a **Base object**
- Only the **Base part** of `d` is copied
- The **Derived part is discarded**

👉 `y` no longer exists

---

### Why this is dangerous (polymorphism)

```cpp
class Base {
public:
    virtual void foo() { std::cout << "Base\n"; }
};

class Derived : public Base {
public:
    void foo() override { std::cout << "Derived\n"; }
};

Derived d;
Base b = d; // slicing

b.foo(); // ❌ prints "Base"
```

👉 Even with `virtual`, slicing **breaks polymorphism**

---

### When does slicing happen?

#### 1. Passing by value
```cpp
void func(Base b); // ❌ slicing
```

#### 2. Returning by value
```cpp
Base create(); // ❌ slicing
```

---

### How to avoid it

Use:

```cpp
Base* b = new Derived();
```

or

```cpp
Derived d;
Base& b = d;
```

👉 Polymorphism works only with **pointers or references**

---

### Mental model

> Copying by value cuts off the derived part

---

## Compile-time vs Runtime polymorphism

| Type | Example |
|------|--------|
| Compile-time | templates, overloading |
| Runtime | virtual functions |

---

## Cost of virtual functions

- extra memory (vptr per object)
- indirect function call
- harder to inline

👉 Tradeoff: flexibility vs performance

---

## When to use virtual?

- behavior depends on derived type
- accessed via base pointer/reference

---

## When NOT to use virtual?

- no polymorphism needed
- performance critical
- prefer composition

---

## Important rules

- Always use `override`
- Always use virtual destructor in polymorphic base classes
- Dynamic dispatch works only via pointer/reference

---

## Common mistakes

- forgetting virtual destructor
- object slicing
- missing `override`
- expecting polymorphism with value objects

---

## Interview Questions

1. **What problem do virtual functions solve?**
   - Enable runtime polymorphism

2. **What is dynamic dispatch?**
   - Resolving function calls at runtime based on actual object type

3. **How do they work internally?**
   - Using vtable and vptr to resolve calls

4. **What is vtable?**
   - Table of function pointers for virtual functions

5. **What is override?**
   - Ensures correct overriding at compile time

6. **What is pure virtual function?**
   - Function with `=0` making class abstract

7. **Why virtual destructor?**
   - Ensures correct destruction via base pointer

8. **What is object slicing?**
   - Losing derived part when assigning to base object

9. **Can constructors be virtual?**
   - ❌ No (object not fully constructed yet)

10. **Cost of virtual?**
   - Memory overhead + runtime dispatch

---

## Final mental model

> Virtual = "decide at runtime"
> Non-virtual = "decide at compile-time"