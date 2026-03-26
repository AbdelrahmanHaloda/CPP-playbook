# SRP vs ISP

There is a strong connection between the **Single Responsibility Principle (SRP)** and the **Interface Segregation Principle (ISP)**. In fact, they are quite similar in that they both aim to promote cohesion and reduce unnecessary coupling, but they apply these ideas differently.

## Differences

**1. Scope:**

- **SRP** is applied at the class level. It dictates that a class should only have one reason to change. In other words, a class should only have one responsibility.
- **ISP** is applied at the interface level. It ensures that interfaces are not too broad and that clients (classes that implement the interface) only have to implement methods that are relevant to them.

**2. Target:**

- **SRP** targets implementation details within classes. It’s about designing classes to be cohesive and focused on one task.
- **ISP** targets interface design. It’s about designing interfaces so that classes aren’t forced to depend on methods they don’t use.

---

## Example 1: ISP Violation, SRP Adherence

Here, we model a Printer that can print documents. The class has a clear responsibility (printing functionality), adhering to **SRP**. However, the printer class implements a large interface that forces it to include unrelated methods, such as scanning and faxing, which it doesn’t need, violating **ISP**.

```cpp
#include <iostream>

// Large interface combining multiple unrelated functionalities
class MultiFunctionDevice {
public:
    virtual void printDocument() = 0;
    virtual void scanDocument() = 0;  // Not needed by basic printers
    virtual void faxDocument() = 0;   // Not needed by basic printers
};

// A BasicPrinter class adheres to SRP but violates ISP
class BasicPrinter : public MultiFunctionDevice {
public:
    void printDocument() override {
        std::cout << "Printing document...\n";
    }

    // Forced to implement irrelevant methods
    void scanDocument() override {
        std::cout << "This printer doesn't support scanning.\n";
    }

    void faxDocument() override {
        std::cout << "This printer doesn't support faxing.\n";
    }
};
```

**Explanation**
**1- Adheres to SRP:**

- The BasicPrinter class has one clear responsibility: printing documents.
- The class doesn’t have multiple responsibilities. It won’t need to change unless the printing functionality changes, which aligns with SRP.

**2- Violates ISP:**

- The BasicPrinter class depends on methods it doesn’t need: scanDocument() and faxDocument().
- It is forced to implement unrelated methods due to the large MultiFunctionDevice interface. This is a classic ISP violation.

## Example 2:How to Fix the ISP Violation

We can break down the MultiFunctionDevice interface into smaller, more specific interfaces to solve the ISP violation.

```cpp
#include <iostream>

// Segregated interfaces
class Printable {
public:
    virtual void printDocument() = 0;
};

class Scannable {
public:
    virtual void scanDocument() = 0;
};

class Faxable {
public:
    virtual void faxDocument() = 0;
};

// BasicPrinter now depends only on the Printable interface
class BasicPrinter : public Printable {
public:
    void printDocument() override {
        std::cout << "Printing document...\n";
    }
};

int main() {
    BasicPrinter printer;
    printer.printDocument();  // Output: Printing document...
    return 0;
}
```

**Explanation**
**1- SRP Adherence:**

- The BasicPrinter class continues to have one responsibility: printing documents.

**2- ISP Adherence:**

- By segregating the interfaces, each class only depends on what it needs:
BasicPrinter only implements the Printable interface.
- If we add a new Scanner class, it will implement only the Scannable interface, and so on.

**Summary**
This example shows how a class can adhere to **SRP** but still violate **ISP** if it’s forced to implement methods it doesn’t need. Fixing the **ISP** violation by breaking down interfaces not only makes the code more modular but also ensures each class depends only on relevant functionality, enhancing maintainability.

---

**Example 3: adhere to ISP and violates SRP**
In this example, we have several segregated interfaces, each representing a distinct responsibility (printing, scanning, faxing). While this design follows **ISP** by ensuring that each class only depends on the methods it requires, the problem lies in the implementation of the OfficePrinter class, which takes on multiple responsibilities, violating **SRP**.

```cpp
#include <iostream>

// Segregated interfaces adhering to ISP
class Printable {
public:
    virtual void printDocument() = 0;
};

class Scannable {
public:
    virtual void scanDocument() = 0;
};

class Faxable {
public:
    virtual void faxDocument() = 0;
};

// OfficePrinter class implementing multiple interfaces
class OfficePrinter : public Printable, public Scannable, public Faxable {
public:
    void printDocument() override {
        std::cout << "Printing document...\n";
    }

    void scanDocument() override {
        std::cout << "Scanning document...\n";
    }

    void faxDocument() override {
        std::cout << "Sending fax...\n";
    }
};
```

**Explanation**
**1- Adheres to ISP:**

- Each interface (Printable, Scannable, and Faxable) is focused on a specific task, following ISP.
- The OfficePrinter class only implements interfaces it requires—no unnecessary dependencies are introduced.

**2- Violates SRP:**

- The OfficePrinter class takes on multiple responsibilities: printing, scanning, and faxing.
- If any one of these functionalities changes (e.g., we modify how scanning works), the OfficePrinter class will need to be modified, which violates SRP. This makes it harder to maintain and extend the code over time.

**Example 4: How to Fix the SRP Violation**
We can split the responsibilities into smaller, dedicated classes to ensure SRP. Let’s introduce specific printers for each responsibility and use composition to combine them when needed.

```cpp
#include <iostream>

class Printable {
public:
    virtual void printDocument() = 0;
};

class Scannable {
public:
    virtual void scanDocument() = 0;
};

class Faxable {
public:
    virtual void faxDocument() = 0;
};

// Individual classes with single responsibilities
class Printer : public Printable {
public:
    void printDocument() override {
        std::cout << "Printing document...\n";
    }
};

class Scanner : public Scannable {
public:
    void scanDocument() override {
        std::cout << "Scanning document...\n";
    }
};

class FaxMachine : public Faxable {
public:
    void faxDocument() override {
        std::cout << "Sending fax...\n";
    }
};

// OfficePrinter now uses composition instead of multiple responsibilities
class OfficePrinter {
public:
    OfficePrinter(Printable& printer, Scannable& scanner, Faxable& faxMachine)
        : printer_(printer), scanner_(scanner), faxMachine_(faxMachine) {}

    void printDocument() { printer_.printDocument(); }
    void scanDocument() { scanner_.scanDocument(); }
    void faxDocument() { faxMachine_.faxDocument(); }

private:
    Printable& printer_;
    Scannable& scanner_;
    Faxable& faxMachine_;
};

int main() {
    Printer printer;
    Scanner scanner;
    FaxMachine faxMachine;

    OfficePrinter officePrinter(printer, scanner, faxMachine);
    officePrinter.printDocument();  // Output: Printing document...
    officePrinter.scanDocument();   // Output: Scanning document...
    officePrinter.faxDocument();    // Output: Sending fax...

    return 0;
}
```

**Explanation of the Fix**
**1- SRP Adherence:**

- Each class (Printer, Scanner, FaxMachine) now has one responsibility.
- If we need to change the scanning behavior, only the Scanner class is modified, not the entire OfficePrinter.

**2- ISP Adherence:**

- We continue to use segregated interfaces (Printable, Scannable, and Faxable), ensuring no class depends on unnecessary methods.

**3- Composition over Inheritance:**

- Instead of having one large class (OfficePrinter) with multiple responsibilities, we compose it using smaller, single-responsibility objects.

**Summary**
In the original example, the design adhered to **ISP** by using segregated interfaces, but violated **SRP** because the OfficePrinter class was responsible for multiple tasks (printing, scanning, and faxing). By splitting responsibilities into dedicated classes and using **composition**, we now achieve both **SRP** and **ISP**, making the code more modular and maintainable.

---
