### What are lvalues and rvalues?
A good grasp of lvalues and rvalues in C++ is essential for understanding the more advanced concepts of rvalue references and motion semantics.

Every expression in C++ has a type and belongs to a value category. When objects are created, copied or moved during the evaluation of an expression, the compiler uses these value expressions to decide which method to call or which operator to use.

Prior to C++11, there were only two value categories, now there are as many as five of them:

To keep it short, we do not want to go into all categories, but limit ourselves to lvalues and prvalues:

**Lvalues** have an address that can be accessed. They are expressions whose evaluation by the compiler determines the identity of objects or functions.

**Prvalues** do not have an address that is accessible directly. They are temporary expressions used to initialize objects or compute the value of the operand of an operator.

For the sake of simplicity and for compliance with many tutorials, videos and books about the topic, let us refer to prvalues as rvalues from here on.

The two characters l and r are originally derived from the perspective of the assignment operator =, which always expects a rvalue on the right, and which it assigns to a lvalue on the left. In this case, the l stands for left and r for right:

`int i = 42;  // lvalue = rvalue;`

With many other operators, however, this right-left view is not entirely correct. In more general terms, an lvalue is an entity that points to a specific memory location. An rvalue is usually a short-lived object, which is only needed in a narrow local scope. To simplify things a little, one could think of lvalues as named containers for rvalues.

In the example above, the value 42 is an rvalue. It does not have a specific memory address which we know about. The rvalue is assigned to a variable i with a specific memory location known to us, which is what makes it an lvalue in this example.

Using the address operator & we can generate an lvalue from an rvalue and assign it to another lvalue:
`int *j = &i;`


[How to crack the confusing world of lvalues and rvalues in C++? It is easy!](https://medium.com/@dhaneshvb/how-to-crack-the-confusing-world-of-lvalues-and-rvalues-in-c-it-is-easy-61c32ced51ce)

[Lvalues and Rvalues (C++)](https://learn.microsoft.com/en-us/cpp/cpp/lvalues-and-rvalues-visual-cpp?view=msvc-160)

---
### What are lvalue reference ?

- lValue reference is as it sounds exactly, `int& x`;

- lValue reference can't be bind to an rvalue **BUT** a lvalue reference to a const can.

**Example 1**

```
void func(int& x)
{}
int main()
{
    int x;
    func(x);    // works fine
    func(42);   // Error, Can't bind lvalue reference to a rvalue
}
```
**Example 2**
```
void func(const int& x)
{}
int main()
{
    int x;
    func(x);    // works fine
    func(42);   // works fine
}
```

**So, from the above example we can see why the copy constructor and copy assigment operator takes a const lvalue reference so it can handle lvalue and rvalue passed to them !**

---

### What is rvalue reference and why it is introduced in c++11?

- ` int &&X; //rvalue reference`

- rvalue reference can't be bind to lvalue. (that's why move semantics is introduced to resolve this problem).

- rvalue reference is introduced in c++ to enhance the performance of code in the situation of moving resources instead of copying it.

- prior to c++11 when we want to move a resource, we had to copy it and this is copy could be an expensive operation if the resource is a heavy object and by heavy i mean an object that allocates memory on the heap or has a huge size.

- so, basically rvalue reference is used in the move constructor to do 2 things
   1- perform a shallow copy of the memory handler.
   2- perform ownership management by copying the memory handler from the orignal source to it then invalidating the memory handler in the orignal.
This can't be done using lvalue reference since lvalue references can't bind to rvalue except if they are const which will make the step number 2 impossible (invalidating the memory handler in the orignal source).