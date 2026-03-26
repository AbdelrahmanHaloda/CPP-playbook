# Composition

## Why this matters
Composition is one of the most important design tools in C++. It helps build flexible systems by combining smaller parts instead of forcing everything into inheritance hierarchies.

In practice, composition often leads to code that is easier to test, easier to extend, and less tightly coupled.

## Core idea
Composition means building a class using other classes as members.

It is commonly explained as:
- **Composition** → **has-a** relationship
- **Inheritance** → **is-a** relationship

Examples:
- A `Car` **has an** `Engine`
- A `SimulationRunner` **has a** `ScenarioLoader`
- A `Cat` **has a** `Tail`

By contrast:
- A `Cat` **is a** `Mammal`

## Composition vs inheritance
Composition is a closely related alternative to inheritance. Instead of taking traits from a base class, a class is constructed from other objects that provide the required behavior.

There is no absolute rule that composition is always better than inheritance, but composition is often the safer default.

### Prefer inheritance when
- there is a true **is-a** relationship
- polymorphic substitution is required
- the derived type must behave like the base type

### Prefer composition when
- a class is made of smaller parts
- behavior comes from combining multiple components
- you want replaceable or swappable dependencies
- code reuse is needed, but there is no real subtype relationship

## Simple example
```cpp
#include <iostream>
#include <string>

class Engine {
public:
    void start() const {
        std::cout << "Engine started\n";
    }
};

class Car {
private:
    Engine engine_;

public:
    void start() {
        engine_.start();
    }
};

int main() {
    Car car;
    car.start();
}
```

Here, `Car` does not inherit from `Engine`.
It **contains** an `Engine`, which is a composition relationship.

## Real-world engineering examples
- A `Logger` can be composed into a service instead of making every service inherit from a logging base class.
- A `SimulationRunner` can contain a `ScenarioLoader`, `Validator`, and `ResultCollector`.
- A `VehicleController` can use a `BrakeModule`, `SteeringModule`, and `SensorInterface` as members.

This style usually scales better than deep inheritance trees.

## Why composition is often preferred
Composition usually improves design because:
- components can evolve independently
- dependencies can be replaced more easily
- testing becomes simpler
- large inheritance hierarchies are avoided
- behavior can be assembled in a more explicit way

This aligns well with clean design and low coupling.

## Common mistake
A common mistake is using inheritance only for code reuse.

If one class does not truly represent a specialized form of another class, inheritance usually creates a weak and misleading design.

Bad reasoning:
- "I want to reuse some functions, so I will inherit"

Better reasoning:
- "My class needs this capability, so I will include an object that provides it"

## Interview / review questions
1. What is the difference between composition and inheritance?
2. What does **has-a** mean in object-oriented design?
3. Why is composition often preferred over inheritance?
4. Give a real C++ example where composition is better than inheritance.
5. Can composition and inheritance be used together in the same design?

## Answers

1. **Difference between composition and inheritance**
   - Composition: "has-a" relationship → a class is built using other classes as members.
   - Inheritance: "is-a" relationship → a class derives from another and **reuses, extends, or overrides its behavior**.
     - **Reuse**: the derived class automatically gets all accessible members (functions and data) from the base class.
     - **Extend**: the derived class can add new functions or data members on top of the base functionality.
     - **Override**: the derived class can provide its own implementation of virtual functions from the base class (runtime polymorphism).

     Example:
     ```cpp
     class Animal {
     public:
         virtual void speak() const {
             std::cout << "Animal sound\n";
         }
     };

     class Dog : public Animal {
     public:
         void speak() const override { // override behavior
             std::cout << "Woof\n";
         }

         void fetch() const { // extend behavior
             std::cout << "Fetching...\n";
         }
     };
     ```

     Here:
     - `Dog` **reuses** `Animal` interface
     - `Dog` **overrides** `speak()`
     - `Dog` **extends** behavior with `fetch()`

   - Composition focuses on combining behavior, while inheritance focuses on specialization.

2. **What does "has-a" mean?**
   - It means an object contains or uses another object as part of its implementation.
   - Example: A `Car` has an `Engine`.

3. **Why is composition often preferred?**
   - Lower coupling
   - Easier testing (dependencies can be mocked/replaced)
   - More flexible design (components can be swapped)
   - Avoids fragile inheritance hierarchies

4. **Example where composition is better**

   A common mistake is using inheritance just to share functionality.

   ### Bad design (inheritance for reuse)
   ```cpp
   class Logger {
   public:
       void log(const std::string& msg) {
           std::cout << msg << '\n';
       }
   };

   class ServiceA : public Logger {};
   class ServiceB : public Logger {};
   ```

   Problems:
   - Forces all services into a "is-a Logger" relationship (which is incorrect)
   - Hard to change logging behavior (file, network, etc.)
   - Tight coupling to a specific implementation

   ---

   ### Better design (composition)
   ```cpp
   class Logger {
   public:
       void log(const std::string& msg) const {
           std::cout << msg << '\n';
       }
   };

   class ServiceA {
   private:
       Logger logger_;

   public:
       void run() {
           logger_.log("ServiceA running");
       }
   };
   ```

   ---

   ### Even better (dependency injection)
   ```cpp
   class ILogger {
   public:
       virtual void log(const std::string& msg) const = 0;
       virtual ~ILogger() = default;
   };

   class ConsoleLogger : public ILogger {
   public:
       void log(const std::string& msg) const override {
           std::cout << msg << '\n';
       }
   };

   class ServiceA {
   private:
       const ILogger& logger_;

   public:
       explicit ServiceA(const ILogger& logger) : logger_(logger) {}

       void run() {
           logger_.log("ServiceA running");
       }
   };
   ```

   Benefits:
   - `ServiceA` no longer depends on a concrete `Logger`, only on an interface (`ILogger`)
   - The logger can be **changed from outside** (console, file, mock, etc.)
   - Enables easy testing by injecting a mock logger
   - Improves flexibility without changing `ServiceA`

   Key idea:
   - Composition: the class **creates/owns** its dependency
   - Dependency Injection: the class **receives** its dependency from outside

5. **Can composition and inheritance be used together?**
   - Yes. Many real systems use both.
   - Example: A base interface for polymorphism (inheritance) + internal components (composition).
   - This is often the most practical design.

## Related topics
- Inheritance
- Polymorphism
- SOLID principles
- Dependency injection
- Aggregation