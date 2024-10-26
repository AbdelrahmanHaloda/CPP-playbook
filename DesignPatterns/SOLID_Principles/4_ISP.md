## Interface Segregation Principle (ISP)

**Example 1: Violation of ISP**
In this version, the Car interface has multiple methods that aren't relevant to all cars. For example, not all cars have an electric engine, so gasoline cars are forced to implement **`chargeBattery()`** even though they don't use it. This violates the **Interface Segregation Principle (ISP)**.

```
#include <iostream>

class Car {
public:
    virtual void startEngine() = 0;
    virtual void stopEngine() = 0;
    virtual void refuel() = 0;  // Not needed for electric cars
    virtual void chargeBattery() = 0;  // Not needed for gasoline cars
};

class GasolineCar : public Car {
public:
    void startEngine() override { std::cout << "Gasoline engine started.\n"; }
    void stopEngine() override { std::cout << "Gasoline engine stopped.\n"; }
    void refuel() override { std::cout << "Refueling the gasoline car.\n"; }
    void chargeBattery() override {}  // Unused, irrelevant method
};

class ElectricCar : public Car {
public:
    void startEngine() override { std::cout << "Electric engine started.\n"; }
    void stopEngine() override { std::cout << "Electric engine stopped.\n"; }
    void refuel() override {}  // Unused, irrelevant method
    void chargeBattery() override { std::cout << "Charging the electric car.\n"; }
};
```

**Problem:**
- Both GasolineCar and ElectricCar are forced to implement methods they don't need, violating ISP.
- This makes the code less modular and harder to maintain.


**Example 2: Adheres to ISP**
We refactor the design by splitting the Car interface into smaller, specific interfaces, ensuring each car class only implements relevant functionality. Now, electric cars and gasoline cars only depend on the methods they need, following ISP.

```
#include <iostream>

class Engine {
public:
    virtual void startEngine() = 0;
    virtual void stopEngine() = 0;
};

class Refuelable {
public:
    virtual void refuel() = 0;
};

class Rechargeable {
public:
    virtual void chargeBattery() = 0;
};

class GasolineCar : public Engine, public Refuelable {
public:
    void startEngine() override { std::cout << "Gasoline engine started.\n"; }
    void stopEngine() override { std::cout << "Gasoline engine stopped.\n"; }
    void refuel() override { std::cout << "Refueling the gasoline car.\n"; }
};

class ElectricCar : public Engine, public Rechargeable {
public:
    void startEngine() override { std::cout << "Electric engine started.\n"; }
    void stopEngine() override { std::cout << "Electric engine stopped.\n"; }
    void chargeBattery() override { std::cout << "Charging the electric car.\n"; }
};

int main() {
    GasolineCar gasolineCar;
    ElectricCar electricCar;

    testEngine(gasolineCar);  // Output: Gasoline engine started. Gasoline engine stopped.
    testEngine(electricCar);  // Output: Electric engine started. Electric engine stopped.

    gasolineCar.refuel();     // Output: Refueling the gasoline car.
    electricCar.chargeBattery();  // Output: Charging the electric car.

    return 0;
}
```

**Explanation:**
- We split the Car interface into smaller, more focused interfaces: Engine, Refuelable, and Rechargeable.
- Each class now implements only the interfaces it needs:
    - GasolineCar implements Engine and Refuelable.
    - ElectricCar implements Engine and Rechargeable.
- This design adheres to ISP by ensuring that no class is forced to implement unnecessary methods.

---