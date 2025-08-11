### <u>Running Multiple Threads</u>

**Fork-Join Parallelism**
Using threads follows a basic concept called **"fork-join-parallelism"**. 
The basic mechanism of this concept follows a simple three-step pattern:

1- Split the flow of execution into a parallel thread ("fork")
2- Perform some work in both the main thread and the parallel thread
3- Wait for the parallel thread to finish and unite the split flow of execution again ("join")

The following diagram illustrates the basic idea of forking:
![Forking_Diagram](/Users/abdelrahmanhaloda/Desktop/AHossam/REPOS/NanoDegreeCPP/1_Docs/Concurrency/Images/forking.jpg)

In the main thread, the program flow is forked into three parallel branches. In both worker branches, some work is performed - which is why threads are often referred to as "worker threads". Once the work is completed, the flow of execution is united again in the main function using the join() command. In this example, join acts as a barrier where all threads are united. The execution of main is in fact halted, until both worker threads have successfully completed their respective work.

**Example 1:**

```c++
#include <iostream>
#include <thread>
#include <vector>

void printHello()
{
    // perform work
    std::cout << "Hello from Worker thread #" << std::this_thread::get_id() << std::endl;
}

int main()
{
    // create threads
    std::vector<std::thread> threads;
    for (size_t i = 0; i < 5; ++i)
    {
        /* 
        * Copying thread objects causes a compile error
        * std::thread objects cannot be copied, only moved. 
        * Thread objects do not have a copy constructor and thus can not be duplicated.
        * This is to prevent multiple std::thread objects from managing the same thread of execution, which would lead to undefined behavior.
        */
        
        /*
        std::thread t(printHello);
        threads.push_back(t); 
        */

        // moving thread objects will work
        threads.emplace_back(std::thread(printHello));
    }

    // do something in main()
    std::cout << "Hello from Main thread #" << std::this_thread::get_id() << std::endl;

    // call join on all thread objects using a range-based loop
    for (auto &t : threads)
        t.join();

    return 0;
}
```

**Example 1 o/p:**
To solve our problem, we can use the function `emplace_back()` instead of `push_back()`, which internally uses **move semantics** to move our thread object into the vector without making a copy. When executing the code, we get the following output:
```
Hello from Worker thread #Hello from Worker thread #135012492896000135012501288704
Hello from Worker thread #135012476110592
Hello from Worker thread #135012484503296
Hello from Main thread #135012519065408
Hello from Worker thread #135012467717888
```
This is surely not how we intended the console output to look like. When we take a close look at the call to std::cout in the thread function, we can see that it actually consists of three parts: the string "Hello from worker…", the respective thread id and finally the line break at the end. In the output, all three components are completely intermingled. Also, when the program is run several times, the output will look different with each execution. This shows us two important properties of concurrent programs:

1- The order in which threads are executed is **non-deterministic**. Every time a program is executed, there is a chance for a completely different order of execution.

2- Threads may get preempted in the middle of execution and another thread may be selected to run.

These two properties pose a major problem with concurrent applications: 
A program may run correctly for thousands of times and suddenly, due to a particular interleaving of threads, there might be a problem. From a debugging perspective, such errors are very hard to detect as they can not be reproduced easily.

---

### <u>A First Concurrency Bug</u>
**Example 2:**
Let us adjust the program code from the previous example and use a Lambda instead of the function printHello(). Also, we will pass the loop counter i into the Lambda to enforce an individual wait time for each thread. The idea is to prevent the interleaving of text on the command line which we saw in the previous example.

```c++
#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <vector>

int main()
{
    // create threads
    std::vector<std::thread> threads;
    for (size_t i = 0; i < 10; ++i)
    {
        // create new thread from a Lambda
        threads.emplace_back([i]() {

            // wait for certain amount of time
            std::this_thread::sleep_for(std::chrono::milliseconds(10 * i));

            // perform work
            std::cout << "Hello from Worker thread #" << i << std::endl;
        });
    }

    // do something in main()
    std::cout << "Hello from Main thread" << std::endl;

    // call join on all thread objects using a range-based loop
    for (auto &t : threads)
        t.join();

    return 0;
}
```

**Example 2 o/p:**
```
Hello from Worker thread #0
Hello from Main thread
Hello from Worker thread #1
Hello from Worker thread #2
Hello from Worker thread #3
Hello from Worker thread #4
Hello from Worker thread #5
Hello from Worker thread #6
Hello from Worker thread #7
Hello from Worker thread #8
Hello from Worker thread #9
```