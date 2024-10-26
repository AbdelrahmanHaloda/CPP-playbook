## SOLID Principles

SOLID is an acronym representing five fundamental principles of object-oriented programming (OOP) and software design that improve maintainability, scalability, and flexibility.

**1. Single Responsibility Principle (SRP)**
**Definition:** A class should have one and only one reason to change.
**Why:** This reduces the risk of bugs when making changes.
Example: A Report class should only handle report data, not formatting it for PDF. Formatting should go in a separate PDFFormatter class.

**2. Open/Closed Principle (OCP)**
**Definition:** Classes should be open for extension but closed for modification.
**Why:** Avoids altering existing code, which can introduce new bugs.
**Example:** Use inheritance or interfaces. Instead of modifying a class to support new behaviors, create a subclass or add plugins.

**3. Liskov Substitution Principle (LSP)**
**Definition:** Subtypes must be substitutable for their base types without altering the correctness of the program.
**Why:** Ensures derived classes can be used wherever the base class is expected.
**Example:** If Square is a subclass of Rectangle, calling setWidth() on a Square object shouldn’t break any logic that expects a Rectangle.

**4. Interface Segregation Principle (ISP)**
**Definition:** Clients should not be forced to depend on interfaces they don’t use.
**Why:** Reduces unnecessary dependencies and improves modularity.
**Example:** Instead of one big Animal interface with methods like fly() and swim(), split it into smaller interfaces like Flyable and Swimmable.

**5. Dependency Inversion Principle (DIP)**
**Definition:** High-level modules should not depend on low-level modules; both should depend on abstractions.
**Why:** Promotes loose coupling and makes code easier to maintain.
**Example:** Instead of directly depending on a MySQLDatabase class, use an IDatabase interface. The concrete class can be changed without affecting the high-level logic.

---

**Summary:**
These principles together help you design software that is easier to modify, test, and extend without breaking existing functionality.

---