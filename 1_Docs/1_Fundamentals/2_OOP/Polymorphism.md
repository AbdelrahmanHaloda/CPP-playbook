# Polymorphism

## Why this matters

Polymorphism allows one interface to work with multiple concrete types.

👉 It enables:
- flexible design
- extensibility
- decoupling interface from implementation

---

## Core idea

> One interface → multiple behaviors

---

## Types of polymorphism in C++

C++ supports two main types:

---

## 1. Overloading (Static Polymorphism)

### What is it?

Same function name, different signatures.

```cpp
int add(int a, int b);
double add(double a, double b);
```

👉 Resolved at **compile-time**

---

### Key points

- no inheritance required
- no virtual needed
- faster (no runtime cost)

---

## 2. Templates (Static Polymorphism)

### What is it?

Generic programming using types as parameters.

```cpp
template<typename T>
T add(T a, T b) {
    return a + b;
}
```

👉 Compiler generates code for each type

---

### Key points

- highly flexible
- zero runtime overhead
- resolved at compile-time

---

## 3. Virtual Functions & Overriding (Runtime Polymorphism)

### What is it?

Behavior depends on the **actual object type at runtime**.

```cpp
class Shape {
public:
    virtual double area() const = 0;
    virtual ~Shape() = default;
};

class Circle : public Shape {
public:
    double area() const override {
        return 3.14;
    }
};
```

---

### Key requirements

- inheritance relationship
- virtual function in base class
- usage via pointer or reference

---

### Why it works

(see VirtualFunctions.md → vtable explanation)

---

### Example

```cpp
Shape* s = new Circle();
s->area(); // calls Circle::area
```

---

## Static vs Runtime polymorphism

| Type | Mechanism | When resolved |
|------|----------|---------------|
| Static | overloading, templates | compile-time |
| Runtime | virtual functions | runtime |

---

## When to use what?

### Use static polymorphism when:
- performance critical
- behavior known at compile-time

### Use runtime polymorphism when:
- behavior varies by object type
- working with base pointers/references

---

## Common mistakes

- object slicing (passing by value)
- forgetting virtual destructors
- confusing overloading vs overriding
- using inheritance when composition is better

---

## Interview Questions

1. **What is polymorphism?**
   - One interface, multiple behaviors

2. **Static vs runtime?**
   - Static → compile-time (overloading, templates)
   - Runtime → virtual functions

3. **Function overloading?**
   - Same function name, different parameters

4. **Template polymorphism?**
   - Generic code instantiated for different types

5. **Runtime polymorphism?**
   - Function resolved at runtime via virtual functions

6. **Requirements?**
   - inheritance, virtual function, base pointer/reference

7. **Object slicing?**
   - Losing derived part when copying by value

---

## Related topics

- VirtualFunctions.md
- Object_lifecycle.md
- Composition.md
- Templates.md

---

## Final mental model

- Overloading → same name, different inputs (compile-time)
- Templates → same logic, different types (compile-time)
- Virtual → same interface, different behavior (runtime)