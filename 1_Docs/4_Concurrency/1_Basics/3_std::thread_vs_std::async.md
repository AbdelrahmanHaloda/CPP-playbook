# std::thread vs std::async

## std::thread

Is a low-level mechanism for creating and managing threads in C++. When you create a **`std::thread`** object, it immediately starts a **new thread** that executes the specified function independently from the calling **thread**. The programmer has explicit control over the thread's lifecycle, including **starting**, **joining**, and **detaching** the **thread**.

---

## std::async

Is a higher-level concurrency mechanism in C++.
When you create a **`std::async`**, you are essentially creating a **task**, not necessarily a **thread**. This **task** is an abstraction that allows the function to be executed either **asynchronously** (in a new **thread**) or **synchronously** (in the current **thread**) based on the **launch policy**. Therefore it provides more flexibility regarding how and when the function is executed. The key difference is that **`std::async`** can decide whether to run the function **asynchronously** (in a new thread) or **synchronously** (in the current thread), depending on the launch policy specified.

---

## Launch Policies for std::async

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

## Questions

### Q1: What is the core difference between `std::thread` and `std::async`?

**Answer:**

- `std::thread` is a low-level execution primitive → it represents a **thread of execution**.

- `std::async` is a higher-level task abstraction → it represents a **computation** that will produce a result.

**Key distinction:**

- `std::thread` → focuses on **how** work runs.
- `std::async` → focuses on **what** result comes back.

### Q2: Why can `std::async` return a value but `std::thread` cannot?

**Answer:**
Because `std::async` provides a result communication mechanism, while `std::thread` does not.
`std::async` creates:

1. A **shared state**.
2. A `std::future<T>` connected to that state.

The worker thread stores the result in the shared state, and the caller retrieves it using `future.get()`. `std::thread` only gives a thread handle (execution control) with no built-in way to transfer results.

### Q3: What does `std::async` actually return?

**Answer:**
It returns a `std::future<T>`. This is not the result itself, but a **handle** to a result that will be available later.

### Q4: What is a `std::future`?

**Answer:**
A `std::future<T>` is a synchronization and retrieval object that represents a value (or exception) that will become available later.
**Main responsibilities:**

- Wait for result (`wait`).
- Retrieve result (`get`).
- Rethrow exceptions from async execution.

### Q5: What is the “shared state” in `std::async`?

**Answer:**
The shared state is an internal object that stores:

- The result (`T`) or an exception (`std::exception_ptr`).
- Readiness status.
- Synchronization primitives.
It is shared between the **producer** (async task) and the **consumer** (`std::future`).

### Q6: What happens under the hood in `std::async(std::launch::async, f)`?

**Answer:**

1. Determine return type `T`.
2. Create shared state for `T`.
3. Create `std::future<T>` linked to that state.
4. Launch a new thread.
5. In that thread: execute `f()`, store result or exception in shared state, and mark state as ready.
6. `future.get()` retrieves the result (blocking if needed).

### Q7: Why doesn't `std::thread` provide a return value mechanism?

**Answer:**
Because it is intentionally designed as a **minimal execution primitive**. It only handles thread creation and lifecycle (`join`, `detach`). It does not handle result transport, exception propagation, or synchronization of results; these must be implemented manually.

### Q8: How can we return a value using `std::thread`?

**Answer:**
By manually creating a communication channel, for example using `std::promise` + `std::future`.

```cpp
std::promise<int> p;
std::future<int> f = p.get_future();
std::thread t([p = std::move(p)]() mutable {
    p.set_value(42);
});
int result = f.get();
t.join();
```

### Q9: How is `std::async` related to `std::promise` and `std::future`?

**Answer:**
`std::async` is conceptually built on top of `std::promise` (producer) and `std::future` (consumer). Internally, it creates a shared state, runs the callable, stores the result (similar to `promise.set_value()`), and returns a future to access it.

### Q10: How are exceptions handled in `std::async`?

**Answer:**
If the callable throws, the exception is captured as a `std::exception_ptr` in the shared state. When `future.get()` is called, the exception is **rethrown** in the caller thread, allowing safe propagation across threads.

### Q11: Why can't exceptions propagate directly between threads?

**Answer:**
Because each thread has its own **call stack**. Instead, `std::async` catches exceptions in the worker thread, stores them, and rethrows them when the result is requested.

### Q12: What is the role of `future.get()`?

**Answer:**
`get()` blocks until the result is ready (if needed), returns the stored value, or rethrows the stored exception. **Note:** It can be called only once as it consumes the result.

### Q13: What is the difference between `join()` and `future.get()`?

**Answer:**

- `join()`: Waits for thread completion but does not provide a result.
- `future.get()`: Waits for computation completion and retrieves the result or exception.

### Q14: Does `std::async(std::launch::async, ...)` always use a new thread?

**Answer:**
Yes, with `std::launch::async`, execution is guaranteed to be asynchronous (typically on a new thread), though the standard doesn't strictly mandate the internal thread creation mechanism.

### Q15: What is the correct mental model of `std::async`?

**Answer:**
Do not think: *"std::async returns a value from another thread."*
Think: *"std::async runs a task that stores its result in shared state, and returns a future that retrieves it later."*

### Q16: Final rule of thumb

**Answer:**

- Use `std::thread` → when you care about the **thread itself**.
- Use `std::async` → when you care about the **result** of a computation.

---
