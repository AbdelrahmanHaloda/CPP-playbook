# Diamond Problem

## Why this matters

The diamond problem is one of the most important multiple-inheritance pitfalls in C++.

If you understand it well, you understand:
- why multiple inheritance can become dangerous
- why virtual inheritance exists
- how ambiguity can appear in both **data** and **behavior**

---

## Core idea

The diamond problem happens when:
- two classes inherit from the same base class
- another class inherits from both of them

This forms a diamond-shaped inheritance graph.

```text
        Vehicle
       /       \
    Boat       Car
       \       /
    AmphibiousCar
```

---

## The problem

Consider this example:

```cpp
class Vehicle {
public:
    virtual void Move() = 0;
};

class Boat : public Vehicle {
public:
    void Move() override {
        // logic A
    }
};

class Car : public Vehicle {
public:
    void Move() override {
        // logic B
    }
};

class AmphibiousCar : public Boat, public Car {
};

int main() {
    AmphibiousCar amphibious;
    amphibious.Move(); // ❌ ambiguous
}
```

o/p:
```shell
/tmp/o9JxK2zOXk.cpp:33:16: error: request for member 'Move' is ambiguous
   33 |     amphibious.Move();  // Conflict between the Move() API (Boat or Car !!)
      |                ^~~~
/tmp/o9JxK2zOXk.cpp:7:18: note: candidates are: 'virtual void Vehicle::Move()'
    7 |     virtual void Move() = 0;
      |                  ^~~~
/tmp/o9JxK2zOXk.cpp:22:10: note:                 'virtual void Car::Move()'
   22 |     void Move()override{
      |          ^~~~
/tmp/o9JxK2zOXk.cpp:13:10: note:                 'virtual void Boat::Move()'
   13 |     void Move()override{
      |          ^~~~
```


### Why is it ambiguous?

Because `AmphibiousCar` inherits:
- one `Move()` from `Boat`
- one `Move()` from `Car`

So the compiler asks:
> Which `Move()` do you mean?

---

## The deeper issue

The problem is not only function-call ambiguity.

Without virtual inheritance, `AmphibiousCar` contains **two separate `Vehicle` subobjects**:
- one from `Boat`
- one from `Car`

That creates two kinds of ambiguity:

### 1. Behavior ambiguity
Which overridden function should be called?

### 2. Base-subobject duplication
The final object contains two `Vehicle` parts.

This can break:
- polymorphic casting
- shared base state
- object layout assumptions

---

## Visual intuition

Without virtual inheritance:

```text
AmphibiousCar
 ├── Boat
 │    └── Vehicle
 └── Car
      └── Vehicle
```

👉 Two separate `Vehicle` subobjects

With virtual inheritance:

```text
AmphibiousCar
 ├── Boat
 ├── Car
 └── Vehicle   (shared)
```

👉 One shared `Vehicle` subobject

---

## Solution Part 1: Virtual inheritance

### What to do

The intermediate classes must inherit virtually from the common base:

```cpp
class Boat : public virtual Vehicle {
public:
    void Move() override {
        // logic A
    }
};

class Car : public virtual Vehicle {
public:
    void Move() override {
        // logic B
    }
};
```

### Why?

This ensures:
- only one shared `Vehicle` base exists
- no duplicated base subobject
- safe and consistent polymorphic behavior at the object-structure level

👉 Virtual inheritance fixes the **object layout problem**

---

## Solution Part 2: Resolve behavior explicitly

Even after virtual inheritance, `Boat` and `Car` still both provide `Move()`.

So the final class must decide what to do.

```cpp
class AmphibiousCar : public Boat, public Car {
public:
    void Move() override {
        // explicit logic here
        // e.g. choose Car::Move() or Boat::Move()
    }
};
```

### Why?

Because the compiler still needs one final answer for behavior.

👉 Overriding in the final class fixes the **function ambiguity problem**

---

## Full corrected example

```cpp
#include <iostream>

class Vehicle {
public:
    virtual void Move() = 0;
    virtual ~Vehicle() = default;
};

class Boat : public virtual Vehicle {
public:
    void Move() override {
        std::cout << "Boat mode\n";
    }
};

class Car : public virtual Vehicle {
public:
    void Move() override {
        std::cout << "Car mode\n";
    }
};

class AmphibiousCar : public Boat, public Car {
public:
    void Move() override {
        std::cout << "Amphibious mode\n";
    }
};

int main() {
    AmphibiousCar amphibious;
    amphibious.Move();
}
```

---

## Important clarification

Virtual inheritance does **not** mean "virtual function".

They solve different problems:

- `virtual function` → runtime dispatch of behavior
- `virtual inheritance` → sharing one common base subobject

These are different uses of the word **virtual**.

---

## When is multiple inheritance acceptable?

Multiple inheritance is not always wrong.

It is usually safer when:
- base classes are interfaces only
- little or no shared state exists
- you clearly understand ownership and object layout

It is more dangerous when:
- multiple base classes contain data
- the hierarchy becomes deep
- behavior overlaps heavily

---

## Common mistakes

- thinking the problem is only about ambiguous function names
- forgetting that the base subobject may be duplicated
- confusing virtual inheritance with virtual functions
- using multiple inheritance when composition is a better design

---

## Interview Questions

1. **What is the diamond problem in C++?**
   - It is an ambiguity that happens when two classes inherit from the same base class, and another class inherits from both of them.

2. **Why does it happen?**
   - Because the final class may contain two copies of the common base, and may inherit conflicting behavior from both sides.

3. **What problem does virtual inheritance solve?**
   - It ensures there is only one shared base subobject instead of duplicates.

4. **Why is overriding in the final class still needed?**
   - Because the compiler still needs one final implementation when both parent classes provide the same function.

5. **What is the difference between virtual inheritance and virtual functions?**
   - Virtual inheritance solves shared-base duplication.
   - Virtual functions solve runtime dispatch.

6. **When is multiple inheritance acceptable?**
   - Usually when the base classes are interface-like and there is minimal shared state.

---

## Related topics

- VirtualFunctions.md
- Polymorphism.md
- Composition.md

---

## Final mental model

- Diamond problem → duplicated base + ambiguous behavior
- Virtual inheritance → fixes the base duplication
- Final override → fixes the behavior ambiguity