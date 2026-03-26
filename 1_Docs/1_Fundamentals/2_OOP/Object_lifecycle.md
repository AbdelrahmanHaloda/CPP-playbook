# <u>Object Lifecycle</u>

---

## Big Picture

Every object in C++ goes through:

1. Construction
2. Lifetime (usage)
3. Destruction

```cpp
{
    MyClass obj; // constructor called
    // use obj
} // destructor called automatically
```

---

## Stack vs Heap

### Stack Allocation
```cpp
MyClass obj; // automatic lifetime
```
- constructed immediately
- destroyed automatically at scope end

### Heap Allocation
```cpp
MyClass* obj = new MyClass();
delete obj;
```
- constructed when `new` is called
- destroyed only when `delete` is called

⚠️ Forgetting `delete` → memory leak

---

## Constructors

### Types
```cpp
class A {
public:
    A();                // default
    A(int x);           // parameterized
};
```

---

### Member Initializer List (IMPORTANT)

```cpp
class A {
    int x;
public:
    A(int val) : x(val) {}
};
```

Why not this?
```cpp
A(int val) {
    x = val; // assignment, not initialization
}
```

👉 Initializer list:
- avoids default + reassignment
- required for:
  - `const` members
  - references

---

### Initialization Order (TRAP)

```cpp
class A {
    int x;
    int y;
public:
    A() : y(10), x(y) {}
};
```

👉 Actual order = declaration order, NOT initializer list

So:
```cpp
x initialized first → y not initialized yet → bug
```

---

## Destructors

```cpp
class A {
public:
    ~A() {}
};
```

- called automatically for stack objects
- called via `delete` for heap objects

---

## `this` Pointer

Every non-static member function has an implicit pointer:

```cpp
class A {
    int x;
public:
    void set(int x) {
        this->x = x;
    }
};
```

👉 `this` is:
- pointer to current object
- type: `A* const`

---

## Inheritance Lifecycle

### Construction Order

```cpp
Base → Derived
```

### Destruction Order

```cpp
Derived → Base
```

Example:
```cpp
class Base {
public:
    Base() { std::cout << "Base\n"; }
    ~Base() { std::cout << "~Base\n"; }
};

class Derived : public Base {
public:
    Derived() { std::cout << "Derived\n"; }
    ~Derived() { std::cout << "~Derived\n"; }
};
```

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
delete b; // ❌ Derived destructor NOT called
```

👉 Result:
- partial destruction
- resource leak
- undefined behavior

---

### Solution

```cpp
class Base {
public:
    virtual ~Base() = default;
};
```

Now:
```cpp
delete b; // ✅ calls Derived then Base destructor
```

---

### Rule

> If a class is used polymorphically → destructor MUST be virtual

---

## `=default` and `=delete`

```cpp
class A {
public:
    A() = default;          // use compiler-generated
    A(const A&) = delete;   // disable copy
};
```

### Why use them?

- express intent clearly
- control object behavior
- avoid unwanted copies

---

## Common Pitfalls

- Forgetting virtual destructor in base class
- Relying on initializer list order instead of declaration order
- Memory leaks from missing `delete`

---

## Interview Questions

1. **Why should base class destructors be virtual?**
   - To ensure correct destruction of derived objects when deleted via base pointer

2. **Construction/destruction order?**
   - Construction: Base → Derived
   - Destruction: Derived → Base

3. **What is `this`?**
   - Pointer to the current object (`ClassName* const`)

4. **Why initializer lists?**
   - Direct initialization, avoids extra work, required for const/reference members

5. **Initialization vs assignment?**
   - Initialization → object created with value
   - Assignment → value set after creation
