### <u>Mutex Types</u>
A short overview of the different available mutex types is given:

- **mutex:** 
Provides the core functions **`lock()`** and **`unlock()`** and the non-blocking **`try_lock()`** method that returns if the **mutex** is not available.

- **recursive_mutex:** 
Allows multiple acquisitions of the **mutex** from the same **thread**.

- **timed_mutex:** 
Similar to **mutex**, but it comes with two more methods **`try_lock_for()`** and **`try_lock_until()`** that try to acquire the **mutex** for a period of time or until a moment in time is reached.

- **recursive_timed_mutex:** is a combination of **timed_mutex** and **recursive_mutex**.
- 
---

### <u>Using a Mutex To Protect Shared Data</u>
**The mutex entity**

Until now, the methods we have used to pass data between threads were short-term and involved passing an argument **(the promise)** from a **parent thread** to a **worker thread** and then passing a result back to the **parent thread** **(via the future)** once it has become available. The **promise-future** construct **is a non-permanent communication channel for one-time usage.**

We have seen that in order to avoid **data races**, we need to either forego accessing shared data or use it in **read-only access** without mutating the data. Now, we want to look at a way to establish a stable **long-term communication channel** that allows for both sharing and mutation. Ideally, we would like to have a communication protocol that corresponds to voice communication over a radio channel, where the transmitter uses the expression **"over"** to indicate the end of the transmission to the receiver. By using such a protocol, sender and receiver can take turns in transmitting their data. In C++, this concept of taking turns can be constructed by an entity called a **"mutex"** - which stands for **MUtual EXclusion.**

A **data race** requires simultaneous access from two **threads**. If we can guarantee that only a single **thread** at a time can access a particular memory location, **data races** would not occur. In order for this to work, we would need to establish a communication protocol. It is important to note that a **mutex** is not the solution to the **data race** problem per se but merely an enabler for a **thread-safe communication protocol** that has to be implemented and adhered to by the programmer.

![Mutex](/Users/abdelrahmanhaloda/Desktop/AHossam/REPOS/NanoDegreeCPP/1_Docs/Concurrency/Images/Mutex.png)

Let us take a look at how this protocol works: 
Assuming we have a piece of memory **(e.g. a shared variable)** that we want to protect from simultaneous access, we can assign a **mutex** to be the guardian of this particular memory. It is important to understand that a **mutex** is bound to the memory it protects. A **thread 1** who wants to access the protected memory must **"lock"** the **mutex** first. After **thread 1** is **"under the lock"**, a **thread 2** is **blocked** from access to the shared variable, it can not acquire the lock on the **mutex** and is temporarily suspended by the system.

Once the reading or writing operation of **thread 1** is complete, it must **"unlock"** the **mutex** so that **thread 2** can access the memory location. Often, the code which is executed **"under the lock"** is referred to as a **"critical section"**. It is important to note that also read-only access to the shared memory has to lock the **mutex** to prevent a **data race** - which would happen when another **thread**, who might be under the lock at that time, were to modify the data.

When several **threads** were to try to acquire and lock the **mutex**, only one of them would be successful. All other **threads** would automatically be put on hold - just as cars waiting at an intersection for a green light. Once the **thread** who has succeeded in acquiring the **lock** had finished its job and unlocked the **mutex**, a queued **thread** waiting for access would be woken up and allowed to lock the **mutex** to proceed with his read / write operation. If all threads were to follow this protocol, a **data race** would effectively be avoided.

---

**Example 1:**
Introducing data race which leads to undefiend behaviour.

```
#include <iostream>
#include <thread>
#include <vector>
#include <future>
#include<algorithm>

class Vehicle
{
public:
    Vehicle(int id) : _id(id) {}

private:
    int _id;
};

class WaitingVehicles
{
public:
    WaitingVehicles() : _tmpVehicles(0) {}

    // getters / setters
    void printSize()
    {
        std::cout << "#vehicles = " << _tmpVehicles << std::endl;
    }

    // typical behaviour methods
    void pushBack(Vehicle &&v)
    {
        //_vehicles.push_back(std::move(v)); // data race would cause an exception
        int oldNum = _tmpVehicles;
        std::this_thread::sleep_for(std::chrono::microseconds(1)); // wait deliberately to expose the data race
        _tmpVehicles = oldNum + 1;
    }

private:
    std::vector<Vehicle> _vehicles; // list of all vehicles waiting to enter this intersection
    int _tmpVehicles; 
};

int main()
{
    std::shared_ptr<WaitingVehicles> queue(new WaitingVehicles); 
    std::vector<std::future<void>> futures;
    for (int i = 0; i < 1000; ++i)
    {
        Vehicle v(i);
        futures.emplace_back(std::async(std::launch::async, &WaitingVehicles::pushBack, queue, std::move(v)));
    }

    std::for_each(futures.begin(), futures.end(), [](std::future<void> &ftr) {
        ftr.wait();
    });

    queue->printSize();

    return 0;
}
```

**Example 1 o/p:**
The correct output should be 999.
This result is due to **data-race**.
```
#vehicles = 436
```
It seems that not all the vehicles could be added to the queue. But why is that? Note that in the **thread** function **"pushBack"** there is a call to sleep_for, which pauses the **thread** execution for a short time. This is the position where the **data race occurs**: First, the current value of _tmpVehicles is stored in a temporary variable oldNum. While the **thread** is paused, there might (and will) be changes to _tmpVehicles performed by other **threads**. When the execution resumes, the former value of _tmpVehicles is written back, thus invalidating the contribution of all the **threads** who had write access in the mean time. Interestingly, when sleep_for is commented out, the output of the program is the same as with **std::launch::deferred** - at least that will be the case for most of the time when we run the program. But once in a while, there might be a scheduling constellation which causes the bug to expose itself. Apart from understanding the **data race**, **you should take as an advice that introducing deliberate time delays in the testing / debugging phase of development can help expose many concurrency bugs.**

---

### <u>Using mutex to protect data</u>

In its simplest form, using a mutex consists of four straight-forward steps:

**1- Include the mutex header `#include <mutex>`**
**2- Create an `std::mutex`**.
**3- lock the mutex using `lock()` before read/write is called**.
**4- Unlock the mutex after the read/write operation is finished using `unlock()`**.

In order to protect the access to _vehicles from being manipulated by several **threads** at once, a **mutex** has been added to the class as a private data member. In the pushBack function, the **mutex** is locked before a new element is added to the vector and unlocked after the operation is complete.

Note that the **mutex** is also locked in the function printSize just before printing the size of the vector. The reason for this lock is two-fold: 
**First**, we want to prevent a **data race** that would occur when a read-access to the vector and a simultaneous write access (even when under the lock) would occur. 

**Second**, we want to exclusively reserve the standard output to the console for printing the vector size without other threads printing to it at the same time.

**Example 2:**
The below code uses a **mutex** to protect the shared resource.

```
#include <iostream>
#include <thread>
#include <vector>
#include <future>
#include <mutex>
#include<algorithm>

class Vehicle
{
public:
    Vehicle(int id) : _id(id) {}

private:
    int _id;
};

class WaitingVehicles
{
public:
    WaitingVehicles() {}

    // getters / setters
    void printSize()
    {
        _mutex.lock();
        std::cout << "#vehicles = " << _vehicles.size() << std::endl;
        _mutex.unlock();
    }

    // typical behaviour methods
    void pushBack(Vehicle &&v)
    {
        _mutex.lock();
        _vehicles.emplace_back(std::move(v)); // data race would cause an exception
        _mutex.unlock();
    }

private:
    std::vector<Vehicle> _vehicles; // list of all vehicles waiting to enter this intersection
    std::mutex _mutex;
};

int main()
{
    std::shared_ptr<WaitingVehicles> queue(new WaitingVehicles); 
    std::vector<std::future<void>> futures;
    for (int i = 0; i < 1000; ++i)
    {
        Vehicle v(i);
        futures.emplace_back(std::async(std::launch::async, &WaitingVehicles::pushBack, queue, std::move(v)));
    }

    std::for_each(futures.begin(), futures.end(), [](std::future<void> &ftr) {
        ftr.wait();
    });

    queue->printSize();

    return 0;
}
```

**Example 2 o/p:**
When this code is executed, 1000 elements will be in the vector. 
**Data race** has been effectively avoided.
```
#vehicles = 1000
```

**Example 3:**
The code below uses **timed_mutex** for 100 ms for 3 times.
```
#include <iostream>
#include <thread>
#include <vector>
#include <future>
#include <mutex>
#include<algorithm>

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

    // getters / setters
    void printSize()
    {
        _mutex.lock();
        std::cout << "#vehicles = " << _vehicles.size() << std::endl;
        _mutex.unlock();
    }

    // typical behaviour methods
    void pushBack(Vehicle &&v)
    {
        for (size_t i = 0; i < 3; ++i)
        {
            if (_mutex.try_lock_for(std::chrono::milliseconds(100)))
            {
                _vehicles.emplace_back(std::move(v));
                //std::this_thread::sleep_for(std::chrono::milliseconds(10));
                _mutex.unlock();
                break;
            }
            else
            {
                std::cout << "Error! Vehicle #" << v.getID() << " could not be added to the vector" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }

private:
    std::vector<Vehicle> _vehicles; // list of all vehicles waiting to enter this intersection
    std::timed_mutex _mutex;
};

int main()
{
    std::shared_ptr<WaitingVehicles> queue(new WaitingVehicles);
    std::vector<std::future<void>> futures;
    for (int i = 0; i < 1000; ++i)
    {
        Vehicle v(i);
        futures.emplace_back(std::async(std::launch::async, &WaitingVehicles::pushBack, queue, std::move(v)));
    }

    std::for_each(futures.begin(), futures.end(), [](std::future<void> &ftr) {
        ftr.wait();
    });

    queue->printSize();

    return 0;
}
```

**Example 3 o/p:**
```
#vehicles = 1000
```

---

### <u>Deadlock 1</u>
Using **mutexes** can significantly reduce the risk of **data races** as seen in 
the example above. But imagine what would happen if an exception was thrown 
while executing code in the critical section, i.e. between **lock and unlock**. 
In such a case, the **mutex** would remain locked indefinitely and no other 
t**thread** could unlock it - the program would most likely freeze.


**Example 4:**
The below code performs a division of numbers:
No Portection on the **shared rescource.**

```
#include <iostream>
#include <thread>
#include <vector>
#include <future>
#include<algorithm>

double result;

void printResult(int denom)
{
    std::cout << "for denom = " << denom << ", the result is " << result << std::endl;
}

void divideByNumber(double num, double denom)
{
    try
    {
        // divide num by denom but throw an exception if division by zero is attempted
        if (denom != 0) 
        {
            result = num / denom;
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
            printResult(denom);
        }
        else
        {
            throw std::invalid_argument("Exception from thread: Division by zero!");
        }
    }
    catch (const std::invalid_argument &e)
    {
        // notify the user about the exception and return
        std::cout << e.what() << std::endl;
        return; 
    }
}

int main()
{
    // create a number of threads which execute the function "divideByNumber" with varying parameters
    std::vector<std::future<void>> futures;
    for (double i = -5; i <= +5; ++i)
    {
        futures.emplace_back(std::async(std::launch::async, divideByNumber, 50.0, i));
    }

    // wait for the results
    std::for_each(futures.begin(), futures.end(), [](std::future<void> &ftr) {
        ftr.wait();
    });

    return 0;
}
```

**Example 4 o/p:**
```
Exception from thread: Division by zero!
for denom = -5, the result is for denom = -4, the result is 16.6667
16.6667
for denom = -3, the result is 16.6667
for denom = -2, the result is for denom = -1, the result is for denom = 1, the result is 16.666716.6667

16.6667
for denom = 2, the result is 16.6667
for denom = 4, the result is 16.6667
for denom = 5, the result is 16.6667
for denom = 3, the result is 16.6667
```
As can easily be seen, the console output is totally mixed up and some results appear multiple times. There are several issues with this program.

**First** 
The **thread** function writes its result to a global variable which is passed to it by reference. 
This will cause a **data race**, The **sleep_for** function exposes the data race clearly.

**Second** 
The result is printed to the console by several **threads** at the same time, causing the chaotic output.



**Example 5:**
The below code is identical to the above but protection was added to resolve the previous issues.

```
#include <iostream>
#include <thread>
#include <vector>
#include <future>
#include <mutex>
#include<algorithm>

std::mutex mtx;
double result;

void printResult(int denom)
{
    std::cout << "for denom = " << denom << ", the result is " << result << std::endl;
}

void divideByNumber(double num, double denom)
{
    mtx.lock();
    try
    {
        // divide num by denom but throw an exception if division by zero is attempted
        if (denom != 0) 
        {
            result = num / denom;
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
            printResult(denom);
        }
        else
        {
            throw std::invalid_argument("Exception from thread: Division by zero!");
        }
    }
    catch (const std::invalid_argument &e)
    {
        // notify the user about the exception and return
        std::cout << e.what() << std::endl;
        return; 
    }
    mtx.unlock();
}

int main()
{
    // create a number of threads which execute the function "divideByNumber" with varying parameters
    std::vector<std::future<void>> futures;
    for (double i = -5; i <= +5; ++i)
    {
        futures.emplace_back(std::async(std::launch::async, divideByNumber, 50.0, i));
    }

    // wait for the results
    std::for_each(futures.begin(), futures.end(), [](std::future<void> &ftr) {
        ftr.wait();
    });

    return 0;
}
```

**Example 5 o/p:**
The system freezed. !
This problem is one type of **deadlock**, which causes a program to freeze because one **thread** does not release the lock on the **mutex** while all other **threads** are waiting for access indefinitely. 
```
for denom = -5, the result is -10
for denom = -4, the result is -12.5
for denom = -3, the result is -16.6667
for denom = -2, the result is -25
for denom = -1, the result is -50
Exception from thread: Division by zero!
```

---

### <u>Deadlock 2</u>
A second type of **deadlock** is a state in which two or more **threads** are blocked because 
each **thread** waits for the resource of the other **thread** to be released before releasing its resource. 
The result of the **deadlock** is a complete standstill. 
The **thread** therefore usually the whole program is blocked forever. 

**Example 6:**
The following code illustrates the problem:

```
#include <iostream>
#include <thread>
#include <mutex>
 
std::mutex mutex1, mutex2;
 
void ThreadA()
{
    // Creates deadlock problem
    mutex2.lock();
    std::cout << "Thread A" << std::endl;
    mutex1.lock();
    mutex2.unlock();
    mutex1.unlock();
}
 
void ThreadB()
{
    // Creates deadlock problem
    mutex1.lock();
    std::cout << "Thread B" << std::endl;
    mutex2.lock();
    mutex1.unlock();
    mutex2.unlock();
}
 
void ExecuteThreads()
{
    std::thread t1( ThreadA );
    std::thread t2( ThreadB );
 
    t1.join();
    t2.join();
 
    std::cout << "Finished" << std::endl;
}
 
int main()
{
    ExecuteThreads();
 
    return 0;
}
```

**Example 6 o/p:**
The system is also freezed. !
Notice that it does not print the **"Finished"** statement nor does it return.
The program is in a **deadlock**, which it can never leave.
```
Thread A
Thread B
```

Let us take a closer look at this problem:

ThreadA and ThreadB both require access to the console. Unfortunately, they request this resource which is protected 
by two **mutexes** in different order. 
If the two **threads** work interlocked so that first **ThreadA** locks **mutex 1**, then **ThreadB** locks **mutex 2**, 
the program is in a deadlock: Each thread tries to lock the other **mutex** and needs to wait for its release, which never comes. 
The following figure illustrates the problem graphically.

![deadLock](/Users/abdelrahmanhaloda/Desktop/AHossam/REPOS/NanoDegreeCPP/1_Docs/Concurrency/Images/DeadLock.png)

**Example 7:**
The below code resolve the deadlock issue.

```
#include <iostream>
#include <thread>
#include <mutex>
 
std::mutex mutex1, mutex2;
 
void ThreadA()
{
    // Solves deadlock problem
    mutex1.lock();
    std::cout << "Thread A" << std::endl;
    mutex2.lock();
    mutex2.unlock();
    mutex1.unlock();
}
 
void ThreadB()
{
    // Solves deadlock problem
    mutex1.lock();
    std::cout << "Thread B" << std::endl;
    mutex2.lock();
    mutex1.unlock();
    mutex2.unlock();
}
 
void ExecuteThreads()
{
    std::thread t1( ThreadA );
    std::thread t2( ThreadB );
 
    t1.join();
    t2.join();
 
    std::cout << "Finished" << std::endl;
}
 
int main()
{
    ExecuteThreads();
 
    return 0;
}
```

**Example 7 o/p:**
Problem Resolved.
Avoiding such a deadlock is possible but requires time and a great deal of experience. 
```
Thread B
Thread A
Finished
```

---

### <u>Using Locks to Avoid Deadlocks</u>

**Lock Guard**

In the previous example, we have directly called the **`lock()`** and **`unlock()`** functions of a **mutex**. The idea of "working under the lock" is to block unwanted access by other **threads** to the same resource. Only the **thread** which acquired the lock can unlock the **mutex** and give all remaining **threads** the chance to acquire the lock. 
In practice however, direct calls to **lock()** should be avoided at all cost! Imagine that while working under the lock, a **thread** would throw an **exception** and exit the critical section without calling the unlock function on the **mutex**. 

In such a situation, the program would most likely **freeze** as no other **thread** could acquire the **mutex** any more. 
This is exactly what we have seen in the function divideByNumber from the previous example.

We can avoid this problem by creating a **`std::lock_guard`** object, which keeps an associated **mutex** locked during the entire object life time. The lock is acquired on construction and released automatically on destruction. 

This makes it impossible to forget unlocking a critical section. Also, **`std::lock_guard`** guarantees **exception safety** because any critical section is automatically unlocked when an **exception is thrown**. 

**Example 8:**
The below code uses  **`std::lock_guard`**

```
#include <iostream>
#include <thread>
#include <vector>
#include <future>
#include <mutex>
#include<algorithm>

std::mutex mtx;
double result;

void printResult(int denom)
{
    std::cout << "for denom = " << denom << ", the result is " << result << std::endl;
}

void divideByNumber(double num, double denom)
{
    try
    {
        // divide num by denom but throw an exception if division by zero is attempted
        if (denom != 0) 
        {
            std::lock_guard<std::mutex> lck(mtx);
            result = num / denom;
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
            printResult(denom);
        }
        else
        {
            throw std::invalid_argument("Exception from thread: Division by zero!");
        }
    }
    catch (const std::invalid_argument &e)
    {
        // notify the user about the exception and return
        std::cout << e.what() << std::endl;
        return; 
    }
}

int main()
{
    // create a number of threads which execute the function "divideByNumber" with varying parameters
    std::vector<std::future<void>> futures;
    for (double i = -5; i <= +5; ++i)
    {
        futures.emplace_back(std::async(std::launch::async, divideByNumber, 50.0, i));
    }

    // wait for the results
    std::for_each(futures.begin(), futures.end(), [](std::future<void> &ftr) {
        ftr.wait();
    });

    return 0;
}
```

**Example 8 o/p:**
Note that there is no direct call to lock or unlock the mutex anymore. We now have a **`std::lock_guard`** object that takes the **mutex** as an argument and locks it at creation. When the method divideByNumber exits, the **mutex** is automatically unlocked by the **`std::lock_guard`** object as soon as it is destroyed - which happens, when the local variable gets out of scope.
```
Exception from thread: Division by zero!
for denom = -5, the result is -10
for denom = -4, the result is -12.5
for denom = -3, the result is -16.6667
for denom = -2, the result is -25
for denom = -1, the result is -50
for denom = 1, the result is 50
for denom = 2, the result is 25
for denom = 3, the result is 16.6667
for denom = 4, the result is 12.5
for denom = 5, the result is 10
```

---

### <u>Unique Lock</u>
The problem with the previous example is that we can only lock the **mutex** once and the only way to control lock and unlock is by invalidating the scope of the **`std::lock_guard`** object. But what if we wanted (or needed) a finer control of the locking mechanism?

A more flexible alternative to **`std::lock_guard`** is **unique_lock**, that also provides support for more advanced mechanisms, such as **deferred locking**, **time locking**, **recursive locking**, **transfer of lock ownership** and use of **condition variables** which behaves similar to **lock_guard** but provides much more flexibility, especially with regard to the timing behavior of the locking mechanism.

**Example 9:**
The below code uses an adapted version of the code from the previous Exmaple:

```
#include <iostream>
#include <thread>
#include <vector>
#include <future>
#include <mutex>
#include<algorithm>

std::mutex mtx;
double result;

void printResult(int denom)
{
    std::cout << "for denom = " << denom << ", the result is " << result << std::endl;
}

void divideByNumber(double num, double denom)
{
    std::unique_lock<std::mutex> lck(mtx);
    try
    {
        // divide num by denom but throw an exception if division by zero is attempted
        if (denom != 0) 
        {   
            result = num / denom;
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
            printResult(denom);
            lck.unlock();

            // do something outside of the lock
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); 

            lck.lock(); 
            // do someting else under the lock
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
        }
        else
        {
            throw std::invalid_argument("Exception from thread: Division by zero!");
        }
    }
    catch (const std::invalid_argument &e)
    {
        // notify the user about the exception and return
        std::cout << e.what() << std::endl;
        return; 
    }
}

int main()
{
    // create a number of threads which execute the function "divideByNumber" with varying parameters
    std::vector<std::future<void>> futures;
    for (double i = -5; i <= +5; ++i)
    {
        futures.emplace_back(std::async(std::launch::async, divideByNumber, 50.0, i));
    }

    // wait for the results
    std::for_each(futures.begin(), futures.end(), [](std::future<void> &ftr) {
        ftr.wait();
    });

    return 0;
}
```

**Example 9 o/p:**
In this version of the code, **`std::lock_guard`** has been replaced with **`std::unique_lock`**. 
As before, the lock object lck will unlock the mutex in its destructor, i.e. when the function divideByNumber returns and lck gets out of scope. 

In addition to this automatic unlocking, **`std::unique_lock`** offers the additional flexibility to engage and disengage the lock as needed by manually calling the methods **`lock()`** and **`unlock()`**. 
This ability can greatly improve the performance of a concurrent program, especially when many **threads** are waiting for access to a locked resource. 

In the example, the lock is released before some non-critical work is performed (simulated by sleep_for) and re-engaged before some other work is performed in the critical section and thus under the lock again at the end of the function. This is particularly useful for optimizing performance and responsiveness when a significant amount of time passes between two accesses to a critical resource.
```
Exception from thread: Division by zero!
for denom = -5, the result is -10
for denom = -4, the result is -12.5
for denom = -3, the result is -16.6667
for denom = -2, the result is -25
for denom = -1, the result is -50
for denom = 1, the result is 50
for denom = 2, the result is 25
for denom = 3, the result is 16.6667
for denom = 4, the result is 12.5
for denom = 5, the result is 10
```
---

**The main advantages of `using std::unique_lock<>` over `std::lock_guard` are briefly summarized in the following.** 

**Using std::unique_lock allows you to:**

- Construct an instance without an associated **mutex** using the default constructor.

- Construct an instance with an associated **mutex** while leaving the mutex unlocked at first using the deferred-locking constructor.

- Construct an instance that tries to lock a **mutex**, but leaves it unlocked if the lock failed using the **try-lock** constructor.
  
- Construct an instance that tries to acquire a lock for either a specified time period or until a specified point in time.


Despite the advantages of **`std::unique_lock<>`** and **`std::lock_guard`** over accessing the **mutex** directly, however, the **deadlock** situation where two **mutexes** are accessed simultaneously will still occur.

---

### <u>Avoiding deadlocks with `std::lock()`</u>
In most cases, your code should only hold one lock on a **mutex** at a time. Occasionally you can nest your locks, for example by calling a subsystem that protects its internal data with a **mutex** while holding a lock on another **mutex**, but it is generally better to avoid locks on multiple **mutexes** at the same time, if possible. Sometimes, however, it is necessary to hold a lock on more than one **mutex** because you need to perform an operation on two different data elements, each protected by its own **mutex**.

We have seen that using several **mutexes** at once can lead to a **deadlock**, if the order of locking them is not carefully managed. 

To avoid this problem, the system must be told that both **mutexes** should be locked at the same time, so that one of the **threads** takes over both locks and blocking is avoided. 
That's what the **std::lock()** function is for - you provide a set of **lock_guard** or **unique_lock** objects and the system ensures that they are all locked when the function returns.

**Example 10:**
In the following example, which is a version of the code we saw in the last section were **`std::mutex`** has been replaced with **`std::lock_guard`**.

```
#include <iostream>
#include <thread>
#include <mutex>
 
std::mutex mutex1, mutex2;
 
void ThreadA()
{
    // Ensure that locks are always executed in the same order
    std::lock(mutex1, mutex2);
    std::lock_guard<std::mutex> lock2(mutex2, std::adopt_lock);
    std::cout << "Thread A" << std::endl;
    std::lock_guard<std::mutex> lock1(mutex1, std::adopt_lock);
    
}
 
void ThreadB()
{
    std::lock(mutex1, mutex2);
    std::lock_guard<std::mutex> lock1(mutex1, std::adopt_lock);
    std::cout << "Thread B" << std::endl;
    std::lock_guard<std::mutex> lock2(mutex2, std::adopt_lock);
}
 
void ExecuteThreads()
{
    std::thread t1( ThreadA );
    std::thread t2( ThreadB );
 
    t1.join();
    t2.join();
 
    std::cout << "Finished" << std::endl;
}
 
int main()
{
    ExecuteThreads();
 
    return 0;
}
```

**Example 10 o/p:**
**Note** that when executing this code, it still produces a deadlock, despite the use of std::lock_guard.
```
Thread A
Thread B

```

**Example 11:**
In the following deadlock-free code, **`std::lock`** is used to ensure that the **mutexes** are always locked in the same order, regardless of the order of the arguments. 
**Note** that **`std::adopt_lock`** option allows us to use **`std::lock_guard`** on an already locked **mutex**.

```

```

**Example 11 o/p:**
```
Thread A
Thread B
Finished
```

As a rule of thumb, programmers should try to avoid using several **mutexes** at once. Practice shows that this can be achieved in the majority of cases. For the remaining cases though, using **std::lock** is a safe way to avoid a **deadlock** situation.

---