// print function?

// Observe the code snippet below. What gets printed?

// Headers
#include <iostream>
int a() { return 2; }


int main()
{
    std::cout << a;
    return 0;
}

/*
1

When you write foo without parentheses, you're referring to the function itself. The function will implicitly convert to a function pointer. 
And then cout will try to print the function pointer. 
But since cout doesn't have a overload method for function pointer, boolean conversion will be apply in such cases. Since the function pointer is not a nullptr, then 1 will be the output. Some compilers, like clang will emit a warning:

C++
<source>:22:18: warning: address of function 'hello' will always evaluate to 'true' [-Wpointer-bool-conversion]
   22 |     std::cout << hello;
      |               ~~ ^~~~~
<source>:22:18: note: prefix with the address-of operator to silence this warning
   22 |     std::cout << hello;
      |                  ^
      |                  &

Full disclosure: You'll probably never need to know this specifically, but understanding the nuances of boolean conversion can help you elsewhere, for example in understanding that int is implicitly convertible to bool, that anything other than 0 is true, and that bool is also implicitly convertible to int. 

For instance,

C++
bool a = true, b = true;
auto c = a + b; // c == int

Use these sorts of questions as avenues to explore continued interest in concepts that can help differentiate you from your competition. Curiosity is a highly sought-after trait nowadays.

In fact, I once got past an interview by being so certain about the nuances of floating point arithmetic, that I corrected the interviewer. He was extremely (but incorrectly) confident, and I politely explained to him why what he thinks is true sounds correct but actually isn't.
*/