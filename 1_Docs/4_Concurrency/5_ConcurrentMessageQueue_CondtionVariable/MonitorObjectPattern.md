# The Monitor Object Pattern

**The monitor object pattern**
**A design pattern** that synchronizes concurrent method execution to ensure that only one method at a time runs within an object. It also allows an object's methods to cooperatively schedule their execution sequences.

The problem solved by this pattern is based on the observation that many applications contain objects whose methods are invoked concurrently by multiple client **threads**. These methods often modify the state of their objects, for example by adding data to an internal vector.

For such concurrent programs to execute correctly, it is necessary to synchronize and schedule access to the objects very carefully. The idea of a monitor object is to synchronize the access to an object's methods so that only one method can execute at any one time.

---

**Example 1:**
Constructing a Monitor Object.

```cpp
#include <iostream>
#include <thread>
#include <vector>
#include <future>
#include <mutex>
#include <algorithm>

class Vehicle
{
public:
    Vehicle(int id) : _id(id) {}
    int getID() { return _id; }

private:
    int _id;
};

class WaitingVehicles
{
public:
    WaitingVehicles() {}

    void printIDs()
    {
        std::lock_guard<std::mutex> myLock(_mutex); // lock is released when myLock goes out of scope
        for(auto &v : _vehicles)
            std::cout << "   Vehicle #" << v.getID() << " is now waiting in the queue" << std::endl;
        
    }

    void pushBack(Vehicle &&v)
    {
        // perform vector modification under the lock
        std::lock_guard<std::mutex> uLock(_mutex);
        std::cout << "   Vehicle #" << v.getID() << " will be added to the queue" << std::endl; 
        _vehicles.emplace_back(std::move(v));

        // simulate some work
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

private:
    std::vector<Vehicle> _vehicles; // list of all vehicles waiting to enter this intersection
    std::mutex _mutex;
};

int main()
{
    // create monitor object as a shared pointer to enable access by multiple threads
    std::shared_ptr<WaitingVehicles> queue(new WaitingVehicles);

    std::cout << "Spawning threads..." << std::endl;
    std::vector<std::future<void>> futures;
    for (int i = 0; i < 10; ++i)
    {
        // create a new Vehicle instance and move it into the queue
        Vehicle v(i);
        futures.emplace_back(std::async(std::launch::async, &WaitingVehicles::pushBack, queue, std::move(v)));
    }

    std::for_each(futures.begin(), futures.end(), [](std::future<void> &ftr) {
        ftr.wait();
    });

    std::cout << "Collecting results..." << std::endl;
    queue->printIDs();

    return 0;
}
```

**Example 1 o/p:**
As can be seen, the Vehicle objects are added one at a time, with all threads duly waiting for their turn. Then, once all Vehicle objects have been stored, the call to printIDs prints the entire content of the vector all at once.

While the functionality of the **monitor object** we have constructed is an improvement over many other methods that allow passing data to threads, it has one significant **disadvantage**:

The **main thread** has to wait until all **worker threads** have completed their jobs and only then can it access the added data in bulk.

A system which is truly interactive however has to react to events as they arrive - it should not wait until all threads have completed their jobs but instead act immediately as soon as new data arrives.

**In the following, we want to add this functionality to our monitor object.**

```sh
   Vehicle #2 will be added to the queue
   Vehicle #3 will be added to the queue
   Vehicle #4 will be added to the queue
   Vehicle #5 will be added to the queue
   Vehicle #6 will be added to the queue
   Vehicle #7 will be added to the queue
   Vehicle #8 will be added to the queue
   Vehicle #9 will be added to the queue
Collecting results...
   Vehicle #0 is now waiting in the queue
   Vehicle #1 is now waiting in the queue
   Vehicle #2 is now waiting in the queue
   Vehicle #3 is now waiting in the queue
   Vehicle #4 is now waiting in the queue
   Vehicle #5 is now waiting in the queue
   Vehicle #6 is now waiting in the queue
   Vehicle #7 is now waiting in the queue
   Vehicle #8 is now waiting in the queue
   Vehicle #9 is now waiting in the queue
```

---

## Creating an infinite polling loop

While the **pushBack** method is used by the **threads** to add data to the monitor incrementally, **the main thread** uses **printSize** at the end to display all the results at once.

Our goal is to change the code in a way that the **main thread** gets notified every time new data becomes available.

But how can the main thread know whether new data has become available?
**The solution** is to write a new method that regularly checks for the arrival of new data.

**Example 2:**
In the code below, a new method **`dataIsAvailable()`** has been added while **`printIDs()`** has been removed. This method returns true if data is available in the vector and false otherwise. Once the **main thread** has found out via **`dataIsAvailable()`** that new data is in the vector, it can call the method **`popBack()`** to retrieve the data from the monitor object. **Note** that instead of copying the data, it is moved from the vector to the main method.

```cpp
#include <iostream>
#include <thread>
#include <vector>
#include <future>
#include <mutex>
#include <algorithm>

class Vehicle
{
public:
    Vehicle(int id) : _id(id) {}
    int getID() { return _id; }

private:
    int _id;
};

class WaitingVehicles
{
public:
    WaitingVehicles() {
        vechilesCounter = 0;
    }

    bool dataIsAvailable()
    {
        std::lock_guard<std::mutex> myLock(_mutex);
        return !_vehicles.empty();
    }
    int getNumVehicles(){
        std::lock_guard<std::mutex> myLock(_mutex);
        return vechilesCounter;
    }

    Vehicle popBack()
    {
        // perform vector modification under the lock
        std::lock_guard<std::mutex> uLock(_mutex);

        // remove last vector element from queue
        Vehicle v = std::move(_vehicles.back());
        _vehicles.pop_back();
        vechilesCounter--;

        return v; // will not be copied due to return value optimization (RVO) in C++
    }

    void pushBack(Vehicle &&v)
    {
        // simulate some work
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // perform vector modification under the lock
        std::lock_guard<std::mutex> uLock(_mutex);

        // add vector to queue
        std::cout << "   Vehicle #" << v.getID() << " will be added to the queue" << std::endl;
        _vehicles.emplace_back(std::move(v));
        vechilesCounter ++;
    }

private:
    std::vector<Vehicle> _vehicles; // list of all vehicles waiting to enter this intersection
    std::mutex _mutex;
    signed int vechilesCounter;
};

int main()
{
    // create monitor object as a shared pointer to enable access by multiple threads
    std::shared_ptr<WaitingVehicles> queue(new WaitingVehicles);

    std::cout << "Spawning threads..." << std::endl;
    std::vector<std::future<void>> futures;
    for (int i = 0; i < 10; ++i)
    {
        // create a new Vehicle instance and move it into the queue
        Vehicle v(i);
        futures.emplace_back(std::async(std::launch::async, &WaitingVehicles::pushBack, queue, std::move(v)));
    }

    std::cout << "Collecting results..." << std::endl;
    while (true)
    {
        if (queue->dataIsAvailable())
        {
            Vehicle v = queue->popBack();
            std::cout << "   Vehicle #" << v.getID() << " has been removed from the queue" << std::endl;
        }
        // sleep sometime
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (queue->getNumVehicles() == 0)
        {
            break;
        }
    }

    std::for_each(futures.begin(), futures.end(), [](std::future<void> &ftr) {
        ftr.wait();
    });

    std::cout << "Finished processing queue" << std::endl;

    return 0;
}
```

In the main thread, an infinite while-loop is used to frequently poll the **monitor object** and check whether new data has become available. Contrary to before, we will now perform the read operation before the workers are done - so we have to integrate our loop before **`wait()`** is called on the **futures** at the end of **`main()`**. Once a new Vehicle object becomes available, we want to print it within the loop.

**Example 2 o/p:**
From the output it can easily be seen, that adding and removing to and from the **monitor object** is now interleaved. When executed repeatedly, the order of the vehicles will most probably differ between executions.

```sh
Spawning threads...
Collecting results...
   Vehicle #0 will be added to the queue
   Vehicle #1 will be added to the queue
   Vehicle #3 will be added to the queue
   Vehicle #2 will be added to the queue
   Vehicle #4 will be added to the queue
   Vehicle #7 will be added to the queue
   Vehicle #6 will be added to the queue
   Vehicle #8 will be added to the queue
   Vehicle #5 will be added to the queue
   Vehicle #5 has been removed from the queue
   Vehicle #9 will be added to the queue
   Vehicle #9 has been removed from the queue
   Vehicle #8 has been removed from the queue
   Vehicle #6 has been removed from the queue
   Vehicle #7 has been removed from the queue
   Vehicle #4 has been removed from the queue
   Vehicle #2 has been removed from the queue
   Vehicle #3 has been removed from the queue
   Vehicle #1 has been removed from the queue
   Vehicle #0 has been removed from the queue
Finished processing queue
```

---
