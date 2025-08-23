## Dependency Inversion Principle (DIP)

**Example 1: Violation of DIP**
In this example, a Keyboard class is directly used by a Computer class. This violates the **Dependency Inversion Principle (DIP)** because the high-level Computer class depends on a specific low-level module (Keyboard). If we need to use a different input device, such as a Mouse, we must modify the Computer class, making it harder to maintain and extend.

```
#include <iostream>

class Keyboard {
public:
    void input() const {
        std::cout << "Keyboard input received.\n";
    }
};

class Computer {
public:
    Computer(const Keyboard& keyboard) : keyboard_(keyboard) {}

    void processInput() const {
        keyboard_.input();
    }

private:
    const Keyboard& keyboard_;
};
```

**Problem:**
The Computer class is tightly coupled with the Keyboard class.
If we introduce another input device (like a Mouse), we must modify the Computer class.


**Example 2: Adheres to DIP**
We refactor the design by introducing an abstraction. Now, the Computer class depends on the InputDevice interface, which the Keyboard and Mouse classes implement. This makes the code flexible and extensible.

```
#include <iostream>

// Abstraction
class InputDevice {
public:
    virtual ~InputDevice() = default;
    virtual void input() const = 0;
};

// Low-level Module 1: Keyboard
class Keyboard : public InputDevice {
public:
    void input() const override {
        std::cout << "Keyboard input received.\n";
    }
};

// Low-level Module 2: Mouse
class Mouse : public InputDevice {
public:
    void input() const override {
        std::cout << "Mouse input received.\n";
    }
};

// High-level Module: Computer
class Computer {
public:
    Computer(const InputDevice& device) : device_(device) {}

    void processInput() const {
        device_.input();
    }

private:
    const InputDevice& device_;
};

int main() {
    Keyboard keyboard;
    Mouse mouse;

    Computer comp1(keyboard);
    Computer comp2(mouse);

    comp1.processInput();  // Output: Keyboard input received.
    comp2.processInput();  // Output: Mouse input received.

    return 0;
}
```

**Explanation:**
- We introduced an abstraction (InputDevice) that defines a common interface for all input devices.
- Both Keyboard and Mouse implement the InputDevice interface.
- The Computer class now depends on the abstraction (InputDevice) rather than the concrete implementations (Keyboard or Mouse).
- This design adheres to DIP, as the high-level Computer class doesn’t rely on specific input devices, making the system more flexible and extensible.

---