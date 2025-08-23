## friend keyword
In C++, friend classes provide an alternative inheritance mechanism to derived classes. The main difference between classical inheritance and friend inheritance is that a friend class can access private members of the base class, which isn't the case for classical inheritance. In classical inheritance, a derived class can only access public and protected members of the base class.

### Friend Functions
You can declare a non-member function as a friend to give it access to a class's private data. This is most commonly used for operator overloading.

Example: Overloading the << Operator
To print an object with std::cout << myObject;, the << operator must be a non-member function because the left-hand operand is std::cout (an object of std::ostream), not your class.


```c++

#include <iostream>

class Box
{
private:
    int _width;

public:
    Box(int width) : _width(width) {}

    // Declare the non-member function as a friend
    friend std::ostream& operator<<(std::ostream& os, const Box& b);
};

// Define the friend function (it is NOT a member of Box)
// It can access Box's private '_width' because it's a friend.
std::ostream& operator<<(std::ostream& os, const Box& b)
{
    os << "Box width: " << b._width;
    return os;
}

int main()
{
    Box myBox(10);
    std::cout << myBox << std::endl; // Works because of our friend function
    return 0;
}
```
---

### Friend Classes
You can declare an entire class as a friend. This gives all of the friend class's member functions access to the private and protected members of the class that granted the friendship.

Example: A Vault and a SecretAgent
A SecretAgent needs access to the Vault's private code, but we don't want anyone else to have it.

```c++

#include <iostream>

class Vault
{
private:
    int _secretCode = 1234;

public:
    // Declare SecretAgent as a friend class
    friend class SecretAgent;
};

class SecretAgent
{
public:
    void revealCode(const Vault& v)
    {
        // This function can access Vault's private members
        // because SecretAgent is a friend of Vault.
        std::cout << "The agent reports the secret code is: " << v._secretCode << std::endl;
    }
};

int main()
{
    Vault a_vault;
    SecretAgent agent_007;
    agent_007.revealCode(a_vault); // The agent can access the code
    return 0;
}
```

---

### Corner Cases and Key Properties

Friendship has very specific rules you must remember.

Friendship is NOT mutual. If class A is a friend of class B, B cannot automatically access A's private members. A must explicitly declare B as its friend.

Friendship is NOT transitive. If A is a friend of B, and B is a friend of C, A is not automatically a friend of C.

Friendship is NOT inherited. If A is a friend of B, a class derived from A is not a friend of B.

---

### Summary

Use the friend keyword sparingly. While useful for operator overloading and tightly coupled classes, it weakens encapsulation. Always consider if there's another design choice before granting friendship.

---