### <u>Starting Threads with Member Functions</u>
We have seen how to start threads with functions and function objects, with and without additional arguments. Also, we now know how to pass arguments to a thread function by reference. 
But 
What if we wish to run a member function other than the function call operator, such as a **member function** of an existing object? Luckily, the C++ library can handle this use-case: For calling member functions, the **std::thread** function requires an additional argument for the object on which to invoke the member function.

**Example 1:**
The Vehicle object v1 is passed to the thread function by value, thus a copy is made which does not affect the "original“ living in the main thread. 
The second Vehicle object v2 is instead passed by reference. 
Therefore, changes to its _id variable will also be visible in the main thread.

```c++
#include <iostream>
#include <thread>

class Vehicle
{
public:
    Vehicle() : _id(0) {}
    void addID(int id) { _id = id; }
    void printID()
    {
        std::cout << "Vehicle ID=" << _id << std::endl;
    }

private:
    int _id;
};

int main()
{
    // create thread
    Vehicle v1, v2;
    std::thread t1 = std::thread(&Vehicle::addID, v1, 1); // call member function on object v
    std::thread t2 = std::thread(&Vehicle::addID, &v2, 2); // call member function on object v

    // wait for thread to finish
    t1.join();
    t2.join();

    // print Vehicle id
    v1.printID();
    v2.printID();

    return 0;
}
```

**Example 1 o/p:**
```shell
Vehicle ID=0
Vehicle ID=2
```
In the previous example, we have to ensure that the existence of v2 outlives the completion of the thread t2 - otherwise there will be an attempt to access an invalidated memory address. An alternative is to use a heap-allocated object and a reference-counted pointer such as std::shared_ptr<Vehicle> to ensure that the object lives as long as it takes the thread to finish its work. The following example shows how this can be implemented:


**Example 2:**
Using a heap-allocated object and a reference-counted pointer to ensure that the object lives as long as it takes the thread to finish its work.
```c++
#include <iostream>
#include <thread>

class Vehicle
{
public:
    Vehicle() : _id(0) {}
    void addID(int id) { _id = id; }
    void printID()
    {
        std::cout << "Vehicle ID=" << _id << std::endl;
    }

private:
    int _id;
};

int main()
{
    // create thread
    std::shared_ptr<Vehicle> v(new Vehicle);
    std::thread t = std::thread(&Vehicle::addID, v, 1); // call member function on object v
    
    // wait for thread to finish
    t.join();
    
    // print Vehicle id
    v->printID();
    
    return 0;
}
```

**Example 2 o/p:**
```shell
Vehicle ID=1
```
---