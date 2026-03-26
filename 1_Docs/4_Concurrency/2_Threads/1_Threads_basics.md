# Concurrency Support in C++11

The concurrency support in C++ makes it possible for a program to execute multiple threads in parallel. Concurrency was first introduced into the standard with C++11. Since then, new concurrency features have been added with each new standard update, such as in C++14 and C++17.

Before C++11, concurrent behavior had to be implemented using native concurrency support from the OS, using **POSIX Threads**, or third-party libraries such as **BOOST**. The standardization of concurrency in C++ now makes it possible to develop cross-platform concurrent programs, which is as significant improvement that saves time and reduces error proneness. Concurrency in C++ is provided by the **thread support library**, which can be accessed by including the header.`#include <thread>`

---

## Thread Creation

Compiling C++ applications that uses `std::thread` library on **Unix-like** system such as linux and macos requires `pthread` library as the underlying implementation rely on it under the hood. This means that the `pthread` library need to be linked when building the program

- To compile using pthread library use pthread flag.

`g++ sourceFile.cpp -pthread`

pthread adds support for multithreading with the pthreads library, and the option sets flags for both the preprocessor and linker:

Now, A running program consists of at least one thread. When the **main function** is executed, we refer to it as the **"main thread"**. Threads are uniquely identified by their thread ID, which can be particularly useful for debugging a program.

**Example 1:**
The code below prints the thread identifier of the main thread and outputs it to the console.
`std::this_thread::get_id()`

```c++
#include <iostream>
#include <thread>

int main()
{
    std::cout << "Hello concurrent world from main! Thread id = " << std::this_thread::get_id() << std::endl;
    return 0;
}
```

**Example 1 o/p:**

```sh
Hello concurrent world from main! Thread id = 1
```

---

**Example 2:**
The code below prints the number of concurrent threads supported to the console.
`uint8_t nThreads = std::thread::hardware_concurrency();`

```c++
#include <iostream>
#include <thread>

int main()
{
    unsigned int nThreads = std::thread::hardware_concurrency();
    std::cout << "This machine supports concurrency with " << nThreads << " concurrent threads available" << std::endl;

    return 0;
}
```

**Example 2 o/p:**

```sh
This machine supports concurrency with 16 concurrent threads available
```

---

## Starting a second thread

To start a second thread in addition to the main thread of the program,

- Create a thread object.

- Pass it the function to be executed by the thread.

Once the thread enters the runnable state, the execution of the associated thread function may start at any point in time.

```c++
// Create a thread object
std::thread thread1(threadFunction);
```

After the thread object has been constructed, the main thread will continue and execute the remaining instructions until it reaches the end and returns. It is possible that by this point in time, the thread will also have finished. But if this is not the case, the main program will terminate and the resources of the associated process will be freed by the OS. As the thread exists within the process, it can no longer access those resources and thus not finish its execution as intended.

To prevent this from happening and have the main program wait for the thread to finish the execution of the thread function, A call to `join()` function on the thread object must be done. This call will only return when the thread reaches the end of the thread function and block the main thread until then.

**Example 3:**

- The code below shows how to use `join()` to ensure that `main()` waits for the thread **thread1** to finish its operations before returning. It uses the function `sleep_for()`, which pauses the execution of the respective threads for a specified amount of time. The idea is to simulate some work to be done in the respective threads of execution.

```c++
#include <iostream>
#include <thread>

void threadFunction()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // simulate work
    std::cout << "Finished work in thread\n"; 
}

int main()
{
    // create thread
    std::thread thread1(threadFunction);

    // do something in main()
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // simulate work
    std::cout << "Finished work in main\n";

    // wait for thread to finish
    thread1.join();

    return 0;
}
```

**Example 3 o/p:**

```sh
Finished work in main
Finished work in thread
```

---

### Q) Why pthread library is needed ?

- **C++ Standard Library and Threads:**
The C++11 standard introduced the `<thread>` library, which provides a high-level interface for creating and managing threads. However, this library is built on top of lower-level threading libraries provided by the operating system.

- **POSIX Threads (pthread):**
On Unix-like operating systems (such as Linux and macOS), the lower-level threading implementation is provided by the **POSIX threads (pthread)** library. The C++ `<thread>` library uses pthread functions **(like pthread_create)** internally to create and manage threads.

- **Linking pthread:**
When you use `<thread>` in your C++ program, the compiler needs to link against the pthread library to resolve the references to the underlying pthread functions. **The -pthread flag does this by:**

  - Defining **_REENTRANT** for the preprocessor, which ensures the standard library headers include thread-safe versions of their function.
  - Adding the **-lpthread** option to the linker, which links the pthread library.

---

### Randomness of events

One very important trait of concurrent programs is their **non-deterministic behavior**. It can not be predicted which thread the scheduler will execute at which point in time.

**Example 4:**
In the code below, the amount of work to be performed both in the thread function and in main has been split into two separate jobs.

```c++
#include <iostream>
#include <thread>

void threadFunction()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // simulate work
    std::cout << "Finished work 1 in thread\n"; 

    std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
    std::cout << "Finished work 2 in thread\n"; 
}

int main()
{
    // create thread
    std::thread t(threadFunction);

    // do something in main()
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // simulate work
    std::cout << "Finished work 1 in main\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
    std::cout << "Finished work 2 in main\n";
    
    // wait for thread to finish
    t.join();

    return 0;
}
```

**Example 4 o/P:**

**- First try:**

```sh
Finished work 1 in thread
Finished work 1 in main
Finished work 2 in thread
Finished work 2 in main
```

**- Second try:**

```sh
Finished work 1 in main
Finished work 1 in thread
Finished work 2 in thread
Finished work 2 in main
```

Executing the code several times more shows that the two versions of program output interchange in a seemingly random manner. This element of randomness is an important characteristic of concurrent programs and we have to take measures to deal with it in a controlled way that prevent unwanted behavior or even program crashes.

---

### Using join() as a barrier

In the previous example (Example 4), the order of execution is determined by the scheduler. If we wanted to ensure that the thread function completed its work before the main function started its own work (because it might be waiting for a result to be available), we could achieve this by repositioning the call to join.

**Example 5:**
In the the below code, the `.join()` has been moved to before the work in main().

```c++
#include <iostream>
#include <thread>

void threadFunction()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // simulate work
    std::cout << "Finished work 1 in thread\n"; 

    std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
    std::cout << "Finished work 2 in thread\n"; 
}

int main()
{
    // create thread
    std::thread t(threadFunction);
    
    // wait for thread to finish
    t.join();

    // do something in main()
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // simulate work
    std::cout << "Finished work 1 in main\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
    std::cout << "Finished work 2 in main\n";

    return 0;
}
```

**Example 5 o/p:**
The order of execution now always looks like the following:

```sh
Finished work 1 in thread
Finished work 2 in thread
Finished work 1 in main
Finished work 2 in main
```

---

### Detach

What happens if we don’t join a thread before its destructor is called. When we comment out join in the example above (Example 5) and then run the program again, it aborts with an error.

The reason why this is done is that the designers of the C++ standard wanted to make debugging a multi-threaded program easier: Having the program crash forces the programer to remember joining the threads that are created in a proper way. Such a hard error is usually much easier to detect than soft errors that do not show themselves so obviously.

There are some situations however, where it might make sense to not wait for a thread to finish its work. This can be achieved by **"detaching"** the thread, by which the internal state variable **"joinable"** is set to **"false"**. This works by calling the `detach()` method on the thread. **The destructor of a detached thread does nothing: It neither blocks nor does it terminate the thread**.

**Example 6:**
detach is called on the thread object, which causes the main thread to immediately continue until it reaches the end of the program code and returns.
**Note** that a detached thread can not be joined ever again.

```c++
#include <iostream>
#include <thread>

void threadFunction()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // simulate work
    std::cout << "Finished work in thread\n"; 
}

int main()
{
    // create thread
    std::thread t(threadFunction);

    // detach thread and continue with main
    t.detach();

    // do something in main()
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // simulate work
    std::cout << "Finished work in main\n";

    return 0;
}
```

**Example 6 o/p:**

```sh
Finished work in main
Finished work in thread // no error !
```

Programmers should be very careful though when using the detach()-method. You have to make sure that the thread does not access any data that might get out of scope or be deleted. Also, we do not want our program to terminate with threads still running. Should this happen, such threads will be terminated very harshly without giving them the chance to properly clean up their resources - what would usually happen in the destructor.

---
