# Starting threads with `std::async`

We know how data can be passed from a worker thread to the parent thread using **promises and futures**. A disadvantage of the **promise-future** approach is that it is very cumbersome (and involves a lot of boilerplate code) to pass the **promise** to the thread function using an **rvalue reference** and **std::move**. For the straight-forward task of returning data or exceptions from a worker thread to the parent thread however, there is a simpler and more convenient way using **`std::async()`** instead of **`std::thread()`**.

---

## std::async and Task Generation

When you use **`std::async`**, you are essentially creating a **task**, not necessarily a **thread**. This **task** is an **abstraction** that allows the function to be executed either **asynchronously** (in a new **thread**) or **synchronously** (in the current **thread**) based on the **launch policy**.

### Launch Policies and Their Effects

- **std::launch::async:**

This policy forces the task to run **asynchronously**, which typically means creating a new **thread** to execute the function. In this case, **`std::async`** does indeed generate a new **thread** to execute the task.

- **std::launch::deferred:**
This policy defers the execution of the task until its result is needed. The function is executed in the same thread that calls **`get()`** on the **future**. No new **thread** is created; the **task** runs **synchronously**.

- **Default Policy:**
If no specific policy is provided, the implementation decides whether to execute the task **asynchronously** or **defer its execution (synchronously)**.

**Benefits of Task-Based Abstraction**
The task-based abstraction provided by **std::async** offers several benefits:

- **Flexibility:**
It allows the implementation to choose the most efficient execution strategy. For example, if the system is under heavy load, it might choose to execute the task synchronously to avoid creating additional threads.

- **Ease of Use:**
It simplifies code by abstracting away the details of thread management. The programmer can focus on the logic of the task rather than the mechanics of thread creation and synchronization.

- **Deferred Execution:**
By using **`std::launch::deferred`**, **tasks** can be scheduled to run only when their result is needed, potentially improving **performance** in scenarios where the result might never be needed.

---

**Example 1:**
The below code use **`std::async`**:
**Note**

- That **`std::async`** returns a **future**.
- Also, we do not need to call **`join()`** any more like we did with **`std::thread`** because with **async**, the thread destructor will be called automatically - which reduces the risk of a concurrency bug.

```c++
#include <iostream>
#include <thread>
#include <future>
#include <cmath>
#include <memory>

double divideByNumberAsync(double num, double denom)
{
    // print system id of worker thread
    std::cout << "Worker thread id = " << std::this_thread::get_id() <<" Using Async parameter"<<std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // simulate work

    if (denom == 0)
        throw std::runtime_error("Exception from thread#: Division by zero!");

    return num / denom;
}

double divideByNumber(double num, double denom)
{
    // print system id of worker thread
    std::cout << "Worker thread id = " << std::this_thread::get_id() <<" Using deferred parameter"<<std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // simulate work

    if (denom == 0)
        throw std::runtime_error("Exception from thread#: Division by zero!");

    return num / denom;
}


int main()
{
    // print system id of worker thread
    std::cout << "Main thread id = " << std::this_thread::get_id() << std::endl;
    
    // Define num and deno
    double num = 42.0, denom = 2.0;
    
    // Define future
    std::future<double> ftr;
    
    // use async to start a task
    ftr = std::async(std::launch::async, divideByNumberAsync, num, denom);
    
    // use deferred to start a synchronous function
    ftr = std::async(std::launch::deferred, divideByNumber, num, denom);

    // retrieve result within try-catch-block
    try
    {
        double result = ftr.get();
        std::cout << "Result = " << result << std::endl;
    }
    catch (std::runtime_error e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
```

**Example 1 o/p:**

```shell
Main thread id = 140506334226240
Worker thread id = 140506316424960 Using async parameter
Worker thread id = 140506334226240 Using deferred parameter
Result = 21
```

As expected, by adjusting the **launch parameters** of **`std::async`** manually (**async**, **deferred**), we can directly influence wether the associated thread function will be executed **synchronously** or **asynchronously**.

**Summary:**
At this point, let us compare **`std::thread`** with **`std::async`**: Internally.
**`std::async`** creates a **promise**, gets a **future** from it and runs a template function that takes the **promise**, calls our function and then either sets the value or the exception of that **promise** - depending on function behavior. The code used internally by **`std::async`** is more or less identical to the code we used in the previous example, except that this time it has been generated by the compiler and it is hidden from us - which means that the code we write appears much cleaner and leaner. Also, **`std::async`** makes it possible to control the amount of concurrency by passing an **optional launch parameter**, which enforces either **synchronous** or **asynchronous** behavior.

**This ability, especially when left to the system, allows us to prevent an overload of threads, which would eventually slow down the system as threads consume resources for both management and communication**.

If we were to use too many threads, the increased resource consumption would outweigh the advantages of parallelism and slow down the program. By leaving the decision to the system, we can ensure that the number of threads is chosen in a carefully balanced way that optimizes runtime performance by looking at the current workload of the system and the multi-core architecture of the system.

**NOTE**
When using **`std::async`** to lunch tasks that do I/O, manipulate mutex, or in other ways interact with other threads without specifying a **launch policy**, the execution can be **non-deterministic**. The **task** might run **asynchronously** in a **new thread** or **synchronously** in the **calling thread**.
This unpredictability can lead to:

**I/O Operations:**
If the task involves I/O, its performance can be unpredictable. Running I/O operations synchronously could block the main thread or other important tasks, causing delays.

**Mutex Manipulation:**
Mutexes are used to prevent concurrent access to shared resources. If the task that manipulates a mutex runs synchronously, it can lead to deadlocks or contention if other threads are also trying to acquire the same mutex.

**Thread Pool Exhaustion:**
If std::async keeps creating new threads for each I/O-bound task, it can exhaust the system's thread pool, leading to performance degradation.

**Resource Contention:**
Multiple tasks manipulating the same mutex or performing I/O on the same resource can lead to contention, increasing the risk of deadlocks and race conditions.

**Exceptions:**
If an exception is thrown in an asynchronously executed task, it must be properly caught and handled. Uncaught exceptions can lead to unexpected behavior.

**Error Propagation:**
Propagating errors back to the main thread or other parts of the program is more challenging with asynchronous execution.

**Ordering and Coordination:**
Ensuring that tasks run in the correct order and coordinating their execution becomes more difficult.

**Shared State:**
Managing shared state between asynchronously executed tasks and other threads requires careful use of synchronization primitives, which can be error-prone.

**Thread Overhead:**
Creating and managing threads has overhead. For I/O-bound tasks, using thread pools or asynchronous I/O mechanisms can be more efficient.

**Context Switching:** Frequent context switching between threads can degrade performance. Using thread pools or non-blocking I/O can mitigate this issue.

---
