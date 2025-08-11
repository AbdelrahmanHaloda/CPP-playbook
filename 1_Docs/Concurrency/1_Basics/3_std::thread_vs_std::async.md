### <u>std::thread vs std::async</u>

#### std::thread
Is a low-level mechanism for creating and managing threads in C++. When you create a **`std::thread`** object, it immediately starts a **new thread** that executes the specified function independently from the calling **thread**. The programmer has explicit control over the thread's lifecycle, including **starting**, **joining**, and **detaching** the **thread**.

#### std::async

Is a higher-level concurrency mechanism in C++.
When you create a **`std::async`**, you are essentially creating a **task**, not necessarily a **thread**. This **task** is an abstraction that allows the function to be executed either **asynchronously** (in a new **thread**) or **synchronously** (in the current **thread**) based on the **launch policy**. Therefore it provides more flexibility regarding how and when the function is executed. The key difference is that **`std::async`** can decide whether to run the function **asynchronously** (in a new thread) or **synchronously** (in the current thread), depending on the launch policy specified.

---

#### Launch Policies for std::async
When using **`std::async`**, you can specify the launch policy to influence whether the function runs **asynchronously** or **synchronously**:

**Asynch Launch Policy**
**`std::launch::async`** 
Forces the function to run asynchronously in a new thread.

**Synch Launch Policy**
**`std::launch::deferred:`** 
Forces the function to run **synchronously**, meaning it is executed only when its result is needed (e.g., when you call **`get()`** on the returned **`std::future object`**).

**Default Launch Policy:** 
If no policy is specified, the implementation decides whether to run the function asynchronously or synchronously based on internal heuristics.

---

Example: 
```c++
#include <iostream>
#include <thread>
#include <future>

// Function to be executed
void threadFunction() {
    std::cout << "Thread Function Executing" << std::endl;
}

int main() {
    // Using std::thread
    std::thread t(threadFunction);

    t.join(); // Wait for the thread to finish

    // Using std::async with std::launch::async
    std::future<void> asyncFuture = std::async(std::launch::async, threadFunction);
    
    asyncFuture.get(); // Wait for the asynchronous task to finish

    // Using std::async with std::launch::deferred
    std::future<void> deferredFuture = std::async(std::launch::deferred, threadFunction);
    
    // Using std::async with Default Lunch Policy
    std::future<void> defaultFuture = std::async(threadFunction);
    
    deferredFuture.get(); // Execute the function synchronously and wait for it to finish


    defaultFuture.get(); // wait for the result we don't know how the system will execute it. (might be synch in the same thread or asynch in a new thread)

    return 0;
}
```

- **`std::thread`** starts a new thread immediately.
- **`std::async`** with **`std::launch::async`** starts a new thread.
- **`std::async`** with **`std::launch::deferred`** defers the function execution until **`get()`** is called, running it in the current thread.

**Summary:**
**`std::thread:`** Directly creates a new thread, giving explicit control over thread management.
**`std::async:`** Provides more flexibility, allowing the system to decide whether to run the function **asynchronously** or **synchronously** based on the specified **launch policy**.

Adjusting the launch parameters of **`std::async`** gives you control over the execution context, either enforcing asynchronous execution with a new thread or deferring execution until needed.

---