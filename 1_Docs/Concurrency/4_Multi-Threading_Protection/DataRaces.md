### <u>Avoiding Data Races</u>

One of the primary sources of error in concurrent programming are **data races**. They occur, when two **concurrent threads** are accessing the same memory location while at least one of them is modifying (the other thread might be reading or modifying). 
In this scenario, the value at the memory location is completely undefined. Depending on the system scheduler, the second thread will be executed at an unknown point in time and thus see different data at the memory location with each execution. Depending on the type of program, the result might be anything from a **crash** to a **security breach** when data is read by a thread that was not meant to be read, such as a user password or other sensitive information. Such an error is called a **"data race“** because two threads are racing to get access to a memory location first, with the content at the memory location depending on the result of the race.

The following diagram illustrates the principle: 

One thread wants to increment a variable x, whereas the other thread wants to print the same variable. Depending on the timing of the program and thus the order of execution, the printed result might change each time the program is executed.

![DataRaces](/Users/abdelrahmanhaloda/Desktop/AHossam/REPOS/NanoDegreeCPP/1_Docs/Concurrency/Images/DataReaces.png)

In this example, one safe way of passing data to a thread would be to carefully synchronize the two threads using either **`join()`** or the **`promise-future concept`** that can guarantee the availability of a result. Data races are always to be avoided. Even if nothing bad seems to happen, they are a bug and should always be treated as such. Another possible solution for the above example would be to make a copy of the original argument and pass the copy to the thread, thereby preventing the data race.

---
### <u>Passing data to a thread by value</u>

**Example 1:**
In the below code, an instance of the proprietary class Vehicle is created and passed to a thread by value, thus making a copy of it.
```c++
#include <iostream>
#include <thread>
#include <future>

class Vehicle
{
public:
    //default constructor
    Vehicle() : _id(0)
    {
        std::cout << "Vehicle #" << _id << " Default constructor called" << std::endl;
    }

    //initializing constructor
    Vehicle(int id) : _id(id)
    {
        std::cout << "Vehicle #" << _id << " Initializing constructor called" << std::endl;
    }

    // setter and getter
    void setID(int id) { _id = id; }
    int getID() { return _id; }

private:
    int _id;
};

int main()
{
    // create instances of class Vehicle
    Vehicle v0; // default constructor
    Vehicle v1(1); // initializing constructor

    // read and write name in different threads (which one of the above creates a data race?)
    std::future<void> ftr = std::async([](Vehicle v) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // simulate work
        v.setID(2);
    }, v0);

    v0.setID(3);

    ftr.wait();
    std::cout << "Vehicle #" << v0.getID() << std::endl;

    return 0;
}
```

Example 1 o/p:
Passing data to a thread in this way is a clean and safe method as there is no danger of a data race - at least when atomic data types such as integers, doubles, chars or booleans are passed.
```shell
Vehicle #0 Default constructor called
Vehicle #1 Initializing constructor called
Vehicle #3
```


**Example 2:**
The below code illustrates the case of hidden pointer in the Vechile class by adding a new member variable to the Vehicle class, which is a pointer to a string object, as well as the corresponding getter and setter functions.
```c++
#include <iostream>
#include <thread>
#include <future>

class Vehicle
{
public:
    //default constructor
    Vehicle() : _id(0)
    {
        std::cout << "Vehicle #" << _id << " Default constructor called" << std::endl;
    }

    //initializing constructor
    Vehicle(int id) : _id(id)
    {
        std::cout << "Vehicle #" << _id << " Initializing constructor called" << std::endl;
    }

    // setter and getter
    void setID(int id) { _id = id; }
    int getID() { return _id; }

private:
    int _id;
};

int main()
{
    // create instances of class Vehicle
    Vehicle v0; // default constructor
    Vehicle v1(1); // initializing constructor

    // read and write name in different threads (which one of the above creates a data race?)
    std::future<void> ftr = std::async([](Vehicle v) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // simulate work
        v.setID(2);
    }, v0);

    v0.setID(3);

    ftr.wait();
    std::cout << "Vehicle #" << v0.getID() << std::endl;

    return 0;
}
```

**Example 2 o/p:**
The basic program structure is mostly identical to the previous example with the object v0 being copied by value **(shallow copy - default copy constructor)** when passed to the thread function. This time however, even though a copy has been made, the original object v0 is modified, when the thread function sets the new name. This happens because the member _name is a pointer to a string and after copying, even though the pointer variable has been duplicated, it still points to the same location as its value (i.e. the memory location) has not changed.
```shell
Vehicle #0 Default constructor called
Vehicle #1 Initializing constructor called
Vehicle 2
```

---

### <u>Overwriting the copy constructor</u>
**Example 3:**
The below code fix the above issue by adding a piece of code that overwrites the default copy constructor and can be modified to make a customized copy of the data members to perform **"deep copy"**.

```c++
#include <iostream>
#include <thread>
#include <future>

class Vehicle
{
public:
    //default constructor
    Vehicle() : _id(0), _name(new std::string("Default Name"))
    {
        std::cout << "Vehicle #" << _id << " Default constructor called" << std::endl;
    }

    //initializing constructor
    Vehicle(int id, std::string name) : _id(id), _name(new std::string(name))
    {
        std::cout << "Vehicle #" << _id << " Initializing constructor called" << std::endl;
    }

    // copy constructor 
    Vehicle(Vehicle const &src)
    {
        if (&src != this)
        {
            _name = new std::string(*(src._name));
        }
        // QUIZ: Student code STARTS here
    };

    // setter and getter
    void setID(int id) { _id = id; }
    int getID() { return _id; }
    void setName(std::string name) { *_name = name; }
    std::string getName() { return *_name; }

private:
    int _id;
    std::string *_name;
};

int main()
{
    // create instances of class Vehicle
    Vehicle v0;    // default constructor
    Vehicle v1(1, "Vehicle 1"); // initializing constructor

    // launch a thread that modifies the Vehicle name
    std::future<void> ftr = std::async([](Vehicle v) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // simulate work
        v.setName("Vehicle 2");
    },v0);

    v0.setName("Vehicle 3");

    ftr.wait();
    std::cout << v0.getName() << std::endl;

    return 0;
}
```

**Example 3 o/p:**
```shell
Vehicle #0 Default constructor called
Vehicle #1 Initializing constructor called
Vehicle 3
```
---

### <u>Passing data using move semantics</u>
Even though a customized copy constructor can help us to avoid data races, it is also time (and memory) consuming. 

A move constructor enables the resources owned by an **rvalue** object to be moved into an **lvalue** without physically copying it. **Rvalue references** support the implementation of **move semantics**, which enables the programmer to write code that transfers resources (such as dynamically allocated memory) from one object to another.

To make use of **move semantics**, we need to provide a **move constructor** (and optionally a move assignment operator). **Copy and assignment operations** whose sources are **rvalues** automatically take advantage of **move semantics**. Unlike the **default copy constructor** however, **the compiler does not provide a default move constructor.**

**Example 4:**
In the below code, we will use move semantics to implement a more effective way of safely passing data to a thread.

```
#include <iostream>
#include <thread>
#include <future>

class Vehicle
{
public:
    //default constructor
    Vehicle() : _id(0), _name(new std::string("Default Name"))
    {
        std::cout << "Vehicle #" << _id << " Default constructor called" << std::endl;
    }

    //initializing constructor
    Vehicle(int id, std::string name) : _id(id), _name(new std::string(name))
    {
        std::cout << "Vehicle #" << _id << " Initializing constructor called" << std::endl;
    }

    // copy constructor 
    Vehicle(Vehicle const &src)
    {
        //...
        std::cout << "Vehicle #" << _id << " copy constructor called" << std::endl;
    };

    // move constructor 
    Vehicle(Vehicle && src)
    {
        _id = src.getID();
        _name = new std::string(src.getName());

        src.setID(0);
        src.setName("Default Name");

        std::cout << "Vehicle #" << _id << " move constructor called" << std::endl;
    };

    // setter and getter
    void setID(int id) { _id = id; }
    int getID() { return _id; }
    void setName(std::string name) { *_name = name; }
    std::string getName() { return *_name; }

private:
    int _id;
    std::string *_name;
};

int main()
{
    // create instances of class Vehicle
    Vehicle v0;    // default constructor
    Vehicle v1(1, "Vehicle 1"); // initializing constructor

    // launch a thread that modifies the Vehicle name
    std::future<void> ftr = std::async([](Vehicle v) {
        v.setName("Vehicle 2");
    },std::move(v0));

    ftr.wait();
    std::cout << v0.getName() << std::endl;

    return 0;
}
```

**Example 4 o/p:**
I don't why moce constructor called 3 times
it should be 1 time but probably because the handling of std::async with lambda and move semantics !!! not sure.
```
Vehicle #0 Default constructor called
Vehicle #1 Initializing constructor called
Vehicle #0 move constructor called
Vehicle #0 move constructor called
Vehicle #0 move constructor called
Default Name
```

---

### <u>Move semantics and uniqueness</u>
As with the above-mentioned **copy constructor**, passing by value is usually safe - provided that a **deep copy** is made of all the data structures within the object that is to be passed. With **move semantics** , we can additionally use the notion of uniqueness to prevent **data races** by default. In the following example, a **unique_pointer** instead of a raw pointer is used for the string member in the Vehicle class.

**Example 5:**

```
#include <iostream>
#include <thread>
#include <future>
#include <memory>

class Vehicle
{
public:
    //default constructor
    Vehicle() : _id(0), _name(new std::string("Default Name"))
    {
        std::cout << "Vehicle #" << _id << " Default constructor called" << std::endl;
    }

    //initializing constructor
    Vehicle(int id, std::string name) : _id(id), _name(new std::string(name))
    {
        std::cout << "Vehicle #" << _id << " Initializing constructor called" << std::endl;
    }

    // move constructor with unique pointer
    Vehicle(Vehicle && src) : _name(std::move(src._name))
    {
        // move id to this and reset id in source
        _id = src.getID();
        src.setID(0);

        std::cout << "Vehicle #" << _id << " move constructor called" << std::endl;
    };

    // setter and getter
    void setID(int id) { _id = id; }
    int getID() { return _id; }
    void setName(std::string name) { *_name = name; }
    std::string getName() { return *_name; }

private:
    int _id;
    std::unique_ptr<std::string> _name;
};


int main()
{
    // create instances of class Vehicle
    Vehicle v0;    // default constructor
    Vehicle v1(1, "Vehicle 1"); // initializing constructor

    // launch a thread that modifies the Vehicle name
    std::future<void> ftr = std::async([](Vehicle v) {
        v.setName("Vehicle 2");
    },std::move(v0));

    ftr.wait();
    std::cout << v0.getName() << std::endl; // this will now cause an exception

    return 0;
}
```

**Example 5 o/p:**
As can be seen, the **`std::string`** has now been changed to a **unique pointer**, which means that only a single reference to the memory location it points to is allowed. Accordingly, the **move constructor** transfers the **unique pointer** to the worker by using **`std::move`** and thus invalidates the pointer in the main thread. When calling **`v0.getName()`**, an **exception is thrown**, making it clear to the programmer that accessing the data at this point is not permissible - which is the whole point of using a **unique pointer** here as a **data race** will now be effectively prevented.

```
Vehicle #0 Default constructor called
Vehicle #1 Initializing constructor called
Vehicle #0 move constructor called
Vehicle #0 move constructor called
Vehicle #0 move constructor called
Segmentation fault (core dumped)
```

---

**Note:**
By adding delays/sleeps into threads can expose dataraces and help you discovering them.