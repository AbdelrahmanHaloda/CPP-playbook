### <u>Promises and Futures</u>

**The promise - future communication channel**

The methods for passing data to a thread we have discussed so far are both useful during thread construction:

We can either pass arguments to the thread function using variadic templates or we can use a Lambda to capture arguments by value or by reference. 

We want to look at a way to pass data in the opposite direction - **that is from the worker threads back to the parent thread.**

**Example 1:**
The code below illustrates the use of these methods again: [variadic templates, Lambda]
Recap.

```c++
#include <iostream>
#include <thread>

void printMessage(std::string message)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // simulate work
    std::cout << "Thread 1: " << message << std::endl;
}

int main()
{
    // define message
    std::string message = "My Message";

    // start thread using variadic templates
    std::thread t1(printMessage, message);

    // start thread using a Lambda expression
    std::thread t2([message] {
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // simulate work
        std::cout << "Thread 2: " << message << std::endl;
    });

    // thread barrier
    t1.join();
    t2.join();

    return 0;
}
```

**Example 1 o/p:**
```shell
Thread 2: My MessageThread 1: My Message
```
A drawback of these two approaches is that the information flows from the parent thread (main) to the worker threads (t1 and t2). 

We want to look at a way to pass data in the opposite direction - that is from the worker threads back to the parent thread.

In order to achieve this, the threads need to adhere to a strict synchronization protocol. There is such a mechanism available in the C++ standard that we can use for this purpose. This mechanism acts as a **single-use channel** between the threads. The sending end of the channel is called **"promise"** while the receiving end is called **"future"**.

In the C++ standard, the class template **`std::promise`** provides a convenient way to store a **value** or an **exception** that will acquired asynchronously at a later time via a **`std::future`** object. Each **`std::promise`** object **is meant to be used only a single time**.

---

**Example 2:**
The code below, we want to declare a promise which allows for transmitting a string between two threads and modifying it in the process.

```c++
#include <iostream>
#include <thread>
#include <future>

void modifyMessage(std::promise<std::string> && prms, std::string message)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(4000)); // simulate work
    std::string modifiedMessage = message + " has been modified"; 
    prms.set_value(modifiedMessage);
}

int main()
{
    // define message
    std::string messageToThread = "My Message";

    // create promise and future
    std::promise<std::string> prms;
    std::future<std::string> ftr = prms.get_future();

    // start thread and pass promise as argument
    std::thread t(modifyMessage, std::move(prms), messageToThread);

    // print original message to console
    std::cout << "Original message from main(): " << messageToThread << std::endl;

    // retrieve modified message via future and print to console
    std::string messageFromThread = ftr.get();
    std::cout << "Modified message from thread(): " << messageFromThread << std::endl;

    // thread barrier
    t.join();

    return 0;
}
```

**Example 2 o/p:**
```shell
Original message from main(): My Message
Modified message from thread(): My Message has been modified
```
After defining a message, we have to create a suitable promise that can take a string object. 
To obtain the corresponding future, we need to call the method **`get_future()`** on the **promise**. **Promise** and **future** are the two types of the communication channel we want to use to pass a string between threads. The communication channel set up in this manner can only pass a string.

We can now create a thread that takes a function and we will pass it the promise as an argument as well as the message to be modified. **Promises can not be copied, because the promise-future concept is a two-point communication channel for one-time use**. Therefore, we must pass the promise to the thread function using **`std::move`**. The thread will then, during its execution, use the **promise** to pass back the modified message.

The thread function takes the **promise** as an **rvalue reference** in accordance with **move semantics**. After waiting for several seconds, the message is modified and the method **`set_value()`** is called on the promise.

Back in the main thread, after starting the thread, the original message is printed to the console. Then, we start listening on the other end of the communication channel by calling the function **`get()`** on the **future**. **This method will block until data is available - which happens as soon as set_value has been called on the promise (from the thread)**. If the result is movable (which is the case for std::string), it will be moved - otherwise it will be copied instead. After the data has been received (with a considerable delay), the modified message is printed to the console.


It is also possible that the worker value calls **`set_value`** on the promise before **`get()`** is called on the **future**. In this case, **`get()`** returns immediately without any delay. After **`get()`** has been called once, **the future is no longer usable**. This makes sense as the normal mode of data exchange between **promise** and **future** works with **`std::move`** - and in this case, the data is no longer available in the channel after the first call to **`get()`**. If **`get()`** is called a second time, **an exception is thrown**.

---

### <u>Passing exceptions</u>
The future-promise communication channel may also be used for **passing** exceptions. To do this, the worker thread simply sets an **exception** rather than a value in the **promise**. In the parent thread, the **exception** is then **re-thrown** once **`get()`** is called on the **future**.

**Example 3:**

```c++
#include <iostream>
#include <thread>
#include <future>
#include <cmath>
#include <memory>

void divideByNumber(std::promise<double> &&prms, double num, double denom)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // simulate work
    try
    {
        if (denom == 0)
            throw std::runtime_error("Exception from thread: Division by zero!");
        else
            prms.set_value(num / denom);
    }
    catch (...)
    {
        prms.set_exception(std::current_exception());
    }
}

int main()
{
    // create promise and future
    std::promise<double> prms;
    std::future<double> ftr = prms.get_future();

    // start thread and pass promise as argument
    double num = 42.0, denom = 0.0;
    std::thread t(divideByNumber, std::move(prms), num, denom);

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

    // thread barrier
    t.join();

    return 0;
}
```

**Example 3 o/p:**
```shell
Exception from thread: Division by zero!
```
In the thread function, we need to implement a try-catch block which can be set to catch a particular **exception** or - as in our case - to catch all exceptions. Instead of setting a value, we now want to throw a **`std::exception`** along with a customized error message. In the catch-block, we catch this exception and throw it to the parent thread using the **promise** with **`set_exception()`**. The function **`std::current_exception`** allows us to easily retrieve the exception which has been thrown.

On the parent side, we now need to catch this exception. In order to do this, we can use a try-block around the call to **`get()`**. We can set the catch-block to catch all exceptions or - as in this example - we could also catch a particular one such as the standard exception. Calling the method **`what()`** on the exception allows us to retrieve the message from the exception - which is the one defined on the promise side of the communication channel.

When we run the program, we can see that the exception is being thrown in the worker thread with the main thread printing the corresponding error message to the console.

**So a promise future pair can be used to pass either values or exceptions between threads.**

---

### <u>Summary</u>

```c++
/*********** Characteristics of Promise and future ***********/

/*
- They can be used to construct a single-use channel from worker thread to the parent thread to pass data and exceptions.
- Promise and Futrue are template type.
- Passing data from the worker thread to the parent thread.
- Single time usage only.
*/

/***************** Promise and future Syntax *****************/

// setting up the promise
std::promise<data_type> prms;

// setting up the future
std::future<data_type> ftr = prms.get_future();

// setting the value in promise
prms.set_value(modifiedMessage); // modifiedMessage is the same data_type


// blocking wait until set_value is called on the promise
std::string messageFromThread = ftr.get();

// blocking wait until future is ready
// This is used when we want to seprate the waiting from the actual retriving
std::future<double> ftr = prms.wait();

// blocking wait until future is ready or the timeout is over
std::future<double> ftr = prms.wait_for(timeout_);

auto status = ftr.wait_for(std::chrono::milliseconds(10000));

/* check if timeout is passed and the futur is ready*/
if (status == std::future_status::ready)
{
    std::cout<<"Result = "<< ftr.get()<<std::endl;
}
else
{
    std::cout<<"ERROR !!!!!!"<<std::endl;
}
```

---

