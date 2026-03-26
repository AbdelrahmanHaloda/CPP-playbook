# Passing Arguments using a Variadic Template

We have seen that one way to pass arguments in to the thread function is to package them in a class using the function call operator. Even though this worked well, it would be very cumbersome to write a special class every time we need to pass data to a thread. We can also use a Lambda that captures the arguments and then calls the function.

But there is a simpler way: The thread constructor may be called with a function and all its arguments. That is possible because the thread constructor is a **variadic template that takes multiple arguments**.

Before C++11, classes and functions could only accept a fixed number of arguments, which had to be specified during the first declaration. With variadic templates it is possible to include any number of arguments of any type.

**Example 1:**
Using variadic templates.
```c++
#include <iostream>
#include <thread>
#include <string>

void printID(int id)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::cout << "ID = " << id << std::endl;
    
}

void printIDAndName(int id, std::string name)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "ID = " << id << ", name = " << name << std::endl;
}

int main()
{
    int id = 0; // Define an integer variable

    // starting threads using variadic templates
    std::thread t1(printID, id);
    std::thread t2(printIDAndName, ++id, "MyString");
    std::thread t3(printIDAndName, ++id); // this procudes a compiler error

    // wait for threads before returning
    t1.join();
    t2.join();
    //t3.join();


    return 0;
}
```

**Example 1 o/p:**
As seen in the code example above, a first thread object is constructed by passing it the function printID and an integer argument. Then, a second thread object is constructed with a function printIDAndName, which requires an integer and a string parameter. If only a single argument was provided to the thread when calling printIDAndName, a compiler error would occur (see std::thread t3 in the example) - which is the same type checking we would get when calling the function directly.

```sh
/usr/include/c++/7/thread: In instantiation of ‘struct std::thread::_Invoker<std::tuple<void (*)(int, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >), int> >’:
/usr/include/c++/7/thread:127:22:   required from ‘std::thread::thread(_Callable&&, _Args&& ...) [with _Callable = void (&)(int, std::__cxx11::basic_string<char>); _Args = {int&}]’
example_1.cpp:25:40:   required from here
/usr/include/c++/7/thread:240:2: error: no matching function for call to ‘std::thread::_Invoker<std::tuple<void (*)(int, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >), int> >::_M_invoke(std::thread::_Invoker<std::tuple<void (*)(int, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >), int> >::_Indices)’
  operator()()
  ^~~~~~~~
/usr/include/c++/7/thread:231:4: note: candidate: template<long unsigned int ..._Ind> decltype (std::__invoke((_S_declval<_Ind>)()...)) std::thread::_Invoker<_Tuple>::_M_invoke(std::_Index_tuple<_Ind ...>) [with long unsigned int ..._Ind = {_Ind ...}; _Tuple = std::tuple<void (*)(int, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >), int>]
    _M_invoke(_Index_tuple<_Ind...>)
    ^~~~~~~~~
/usr/include/c++/7/thread:231:4: note:   template argument deduction/substitution failed:
/usr/include/c++/7/thread: In substitution of ‘template<long unsigned int ..._Ind> decltype (std::__invoke(_S_declval<_Ind>()...)) std::thread::_Invoker<std::tuple<void (*)(int, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >), int> >::_M_invoke<_Ind ...>(std::_Index_tuple<_Ind1 ...>) [with long unsigned int ..._Ind = {0, 1}]’:
/usr/include/c++/7/thread:240:2:   required from ‘struct std::thread::_Invoker<std::tuple<void (*)(int, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >), int> >’
/usr/include/c++/7/thread:127:22:   required from ‘std::thread::thread(_Callable&&, _Args&& ...) [with _Callable = void (&)(int, std::__cxx11::basic_string<char>); _Args = {int&}]’
example_1.cpp:25:40:   required from here
/usr/include/c++/7/thread:233:29: error: no matching function for call to ‘__invoke(std::__tuple_element_t<0, std::tuple<void (*)(int, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >), int> >, std::__tuple_element_t<1, std::tuple<void (*)(int, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >), int> >)’
    -> decltype(std::__invoke(_S_declval<_Ind>()...))
                ~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~
In file included from /usr/include/c++/7/tuple:41:0,
                 from /usr/include/c++/7/bits/unique_ptr.h:37,
                 from /usr/include/c++/7/memory:80,
                 from /usr/include/c++/7/thread:39,
                 from example_1.cpp:2:
/usr/include/c++/7/bits/invoke.h:89:5: note: candidate: template<class _Callable, class ... _Args> constexpr typename std::__invoke_result<_Functor, _ArgTypes>::type std::__invoke(_Callable&&, _Args&& ...)
     __invoke(_Callable&& __fn, _Args&&... __args)
     ^~~~~~~~
/usr/include/c++/7/bits/invoke.h:89:5: note:   template argument deduction/substitution failed:
/usr/include/c++/7/bits/invoke.h: In substitution of ‘template<class _Callable, class ... _Args> constexpr typename std::__invoke_result<_Functor, _ArgTypes>::type std::__invoke(_Callable&&, _Args&& ...) [with _Callable = void (*)(int, std::__cxx11::basic_string<char>); _Args = {int}]’:
/usr/include/c++/7/thread:233:29:   required by substitution of ‘template<long unsigned int ..._Ind> decltype (std::__invoke(_S_declval<_Ind>()...)) std::thread::_Invoker<std::tuple<void (*)(int, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >), int> >::_M_invoke<_Ind ...>(std::_Index_tuple<_Ind1 ...>) [with long unsigned int ..._Ind = {0, 1}]’
/usr/include/c++/7/thread:240:2:   required from ‘struct std::thread::_Invoker<std::tuple<void (*)(int, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >), int> >’
/usr/include/c++/7/thread:127:22:   required from ‘std::thread::thread(_Callable&&, _Args&& ...) [with _Callable = void (&)(int, std::__cxx11::basic_string<char>); _Args = {int&}]’
example_1.cpp:25:40:   required from here
/usr/include/c++/7/bits/invoke.h:89:5: error: no type named ‘type’ in ‘struct std::__invoke_result<void (*)(int, std::__cxx11::basic_string<char>), int>’
```

**Example 2:**
There is one more difference between calling a function directly and passing it to a thread: With the former, arguments may be passed by value, by reference or by using move semantics - depending on the signature of the function. When calling a function using a variadic template, the arguments are by default either moved or copied - depending on wether they are rvalues or lvalues. There are ways however which allow us to overwrite this behavior. If you want to move an lvalue for example, we can call std::move. In the following example, two threads are started, each with a different string as a parameter. With t1, the string name1 is copied by value, which allows us to print name1 even after join has been called. The second string name2 is passed to the thread function using move semantics, which means that it is not available any more after join has been called on t2.

```c++

#include <iostream>
#include <thread>
#include <string>

void printName(std::string name, int waitTime)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
    std::cout << "Name (from Thread) = " << name << std::endl;
}

int main()
{
    std::string name1 = "MyThread1";
    std::string name2 = "MyThread2";

    // starting threads using value-copy and move semantics 
    std::thread t1(printName, name1, 50);
    std::thread t2(printName, std::move(name2), 100);

    // wait for threads before returning
    t1.join();
    t2.join();

    // print name from main
    std::cout << "Name (from Main) = " << name1 << std::endl;
    std::cout << "Name (from Main) = " << name2 << std::endl;

    return 0;
}
```

**Example 2 o/p:**
The console output shows how using copy-by-value and std::move affect the string parameters:

```sh
Name (from Thread) = MyThread1
Name (from Thread) = MyThread2
Name (from Main) = MyThread1
Name (from Main) = 
```

**Example 3:**
In the following example, the **signature of the thread function is modified** to take a non-const reference to the string instead.
When passing the string variable name to the thread function, we need to explicitly mark it as a reference, so the compiler will treat it as such. This can be done by using the std::ref function.

```c++
#include <iostream>
#include <thread>
#include <string>

void printName(std::string &name, int waitTime)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
    name += " (from Thread)";
    std::cout << name << std::endl;
}

int main()
{
    std::string name("MyThread");

    // starting thread
    std::thread t(printName, std::ref(name), 50);

    // wait for thread before returning
    t.join();

    // print name from main
    name += " (from Main)";
    std::cout << name << std::endl;

    return 0;
}
```

**Example 3 o/p:**
In the console output it becomes clear that the string has been successfully modified within the thread function before being passed to main.
Even though the code works, we are now sharing mutable data between threads.

```sh
MyThread (from Thread)
MyThread (from Thread) (from Main)
```

---
