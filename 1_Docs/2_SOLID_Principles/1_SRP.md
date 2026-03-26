# Single Responsibility Principle (SRP)

**Example 1: Violating the SRP**
This class handles both storing data and printing it, violating SRP.

```cpp
#include <iostream>
#include <string>

class User {
public:
    User(const std::string& name, int age) : name_(name), age_(age) {}

    void saveToDatabase() {
        std::cout << "Saving " << name_ << " to the database.\n";
    }

    void printUser() {
        std::cout << "User: " << name_ << ", Age: " << age_ << '\n';
    }

private:
    std::string name_;
    int age_;
};

int main() {
    User user("Alice", 30);
    user.saveToDatabase();
    user.printUser();
    return 0;
}
```

**Violation:**
The User class has two responsibilities:
- database operations.
- printing the user data.

If printing logic or database handling changes, we must modify this class.

**Example 2: Adhering to SRP**
In this version, we separate data storage from printing.

```c++
#include <iostream>
#include <string>

class User {
public:
    User(const std::string& name, int age) : name_(name), age_(age) {}

    std::string getName() const { return name_; }
    int getAge() const { return age_; }

private:
    std::string name_;
    int age_;
};

class UserDatabase {
public:
    void saveToDatabase(const User& user) {
        std::cout << "Saving " << user.getName() << " to the database.\n";
    }
};

class UserPrinter {
public:
    void printUser(const User& user) {
        std::cout << "User: " << user.getName() << ", Age: " << user.getAge() << '\n';
    }
};

int main() {
    User user("Alice", 30);
    UserDatabase db;
    UserPrinter printer;

    db.saveToDatabase(user);
    printer.printUser(user);

    return 0;
}
```

**Adherence:**

- UserDatabase handles database operations.
- UserPrinter handles printing.
- User only holds the data.

Now, if printing or database logic changes, we only update the relevant class.

In this example we made the class User adehere to SRP by introducing a 2 new seprate classes that performs these extra functionality (UserPrinter and UserPrinter) and made these classes composed of User class through dependency injection which is a good design.

---
