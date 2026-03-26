# Rvalue references and std::move

lvalues, rvalues and especially rvalue references is an important prerequisite for understanding the concept of moving data structures.

Let us consider the function below which takes an rvalue reference as its parameter.

```c++
#include <iostream>

void myFunction(int &&val)
{
    std::cout << "val = " << val << std::endl;
}

int main()
{
    myFunction(42);

    return 0; 
}
```

The important message of the function argument of myFunction to the programmer is : The object that binds to the rvalue reference &&val is yours, it is not needed anymore within the scope of the caller (which is main). This is interesting from two perspectives:

Passing values like this improves performance as no temporary copy needs to be made anymore and ownership changes, since the object the reference binds to has been abandoned by the caller and now binds to a handle which is available only to the receiver. This could not have been achieved with lvalue references as any change to the object that binds to the lvalue reference would also be visible on the caller side.

There is one more important aspect we need to consider: rvalue references are themselves lvalues. While this might seem confusing at first glance, it really is the mechanism that enables move semantics: A reference is always defined in a certain context (such as in the above example the variable val). Even though the object it refers to (the number 42) may be disposable in the context it has been created (the main function), it is not disposable in the context of the reference. So within the scope of myFunction, val is an lvalue as it gives access to the memory location where the number 42 is stored.

Note however that in the above code example we cannot pass an lvalue to myFunction, because an rvalue reference cannot bind to an lvalue. The code

```c++
int i = 23;
myFunction(i)
```

would result in a compiler error. There is a solution to this problem though: The function std::move converts an lvalue into an rvalue (actually, to be exact, into an xvalue, which we will not discuss here for the sake of clarity), which makes it possible to use the lvalue as an argument for the function:

```c++
int i = 23; 
myFunction(std::move(i));
```

In doing this, we state that in the scope of main we will not use i anymore, which now exists only in the scope of myFunction. Using std::move in this way is one of the components of move semantics.

---

## The Rule of Five

By adding both the move constructor and the move assignment operator to our MyMovableClass, we have adhered to the Rule of Five. This rule is an extension of the Rule of Three which we have already seen and exists since the introduction of the C++11 standard. The Rule of Five is especially important in resource management, where unnecessary copying needs to be avoided due to limited resources and performance reasons. Also, all the STL container classes such as std::vector implement the Rule of Five and use move semantics for increased efficiency.

The Rule of Five states that if you have to write one of the functions listed below then you should consider implementing all of them with a proper resource management policy in place. If you forget to implement one or more, the compiler will usually generate the missing ones (without a warning) but the default versions might not be suitable for the purpose you have in mind. The five functions are:

**The destructor:** Responsible for freeing the resource once the object it belongs to goes out of scope.

**The copy assignment operator:** The default assignment operation performs a member-wise shallow copy, which does not copy the content behind the resource handle. If a deep copy is needed, it has be implemented by the programmer.

**The copy constructor:** As with the assignment operator, the default copy constructor performs a shallow copy of the data members. If something else is needed, the programmer has to implement it accordingly.

**The move constructor:** Because copying objects can be an expensive operation which involves creating, copying and destroying temporary objects, rvalue references are used to bind to an rvalue. Using this mechanism, the move constructor transfers the ownership of a resource from a (temporary) rvalue object to a permanent lvalue object.

**The move assignment operator:** With this operator, ownership of a resource can be transferred from one object to another. The internal behavior is very similar to the move constructor.

### When are move semantics used?

Now that we have seen how move semantics work, let us take a look at situations where they actually apply.

One of the primary areas of application are cases, where heavy-weight objects need to be passed around in a program. Copying these without move semantics can cause series performance issues. The idea in this scenario is to create the object a single time and then "simply" move it around using rvalue references and move semantics.

A second area of application are cases where ownership needs to be transferred (such as with unique pointers, as we will soon see). The primary difference to shared references is that with move semantics we are not sharing anything but instead we are ensuring through a smart policy that only a single object at a time has access to and thus owns the resource.

---

**Example**
Now consider the following main function:

```c++
int main()
{
    MyMovableClass obj1(100); // constructor

    obj1 = MyMovableClass(200); // move assignment operator

    MyMovableClass obj2 = MyMovableClass(300); // move constructor 

    return 0;
}
```

In this version, we also have an instance of MyMovableClass, obj1. Then, a second instance of MyMovableClass is created as an rvalue, which is assigned to obj1. Finally, we have a second lvalue obj2, which is created by assigning it an rvalue object. Let us take a look at the output of the program: 
**with compiler optimization**

```sh
CREATING instance of MyMovableClass at 0x7ffc1ad9e0b0 allocated with size = 400 bytes

CREATING instance of MyMovableClass at 0x7ffc1ad9e0c0 allocated with size = 800 bytes

MOVING (assign) instance 0x7ffc1ad9e0c0 to instance 0x7ffc1ad9e0b0

DELETING instance of MyMovableClass at 0x7ffc1ad9e0c0

CREATING instance of MyMovableClass at 0x7ffc1ad9e0c0 allocated with size = 1200 bytes

DELETING instance of MyMovableClass at 0x7ffc1ad9e0c0
DELETING instance of MyMovableClass at 0x7ffc1ad9e0b0
```

**without compiler optimization** `-fno-elide-constructors`

```sh
CREATING instance of MyMovableClass at 0x7fffa7898740 allocated with size = 400 bytes
CREATING instance of MyMovableClass at 0x7fffa7898760 allocated with size = 800 bytes
MOVING (assign) instance 0x7fffa7898760 to instance 0x7fffa7898740
DELETING instance of MyMovableClass at 0x7fffa7898760
CREATING instance of MyMovableClass at 0x7fffa7898760 allocated with size = 1200 bytes
COPYING content of instance 0x7fffa7898760 to instance 0x7fffa7898750
DELETING instance of MyMovableClass at 0x7fffa7898760
DELETING instance of MyMovableClass at 0x7fffa7898750
DELETING instance of MyMovableClass at 0x7fffa7898740
```

By looking at the stack addresses of the objects, we can see that the temporary object at 0x7ffeefbff708 is moved to 0x7ffeefbff718 using the move assignment operator we wrote earlier, because the instance obj1 is assigned an rvalue. As expected from an rvalue, its destructor is called immediately afterwards. But as we have made sure to null its data pointer in the move constructor, the actual data will not be deleted. The advantage from a performance perspective in this case is that no deep-copy of the rvalue object needs to be made, we are simply redirecting the internal resource handle thus making an efficient shallow copy.

Next, another temporary instance with a size of 1200 bytes is created as a temporary object and "assigned" to obj3. Note that while the call looks like an assignment, the move constructor is called under the hood, making the call identical to MyMovableClass obj2(MyMovableClass(300));. By creating obj3 in such a way, we are reusing the temporary rvalue and transferring ownership of its resources to the newly created obj3.
**Final Example**

```c++
void useObject(MyMovableClass obj)
{
    std::cout << "using object " << &obj << std::endl;
}

int main()
{
    MyMovableClass obj1(100); // constructor

    useObject(obj1);

    MyMovableClass obj2 = MyMovableClass(200);

    useObject(std:: move(obj2));

    return 0;
}
```

In this case, an instance of MyMovableClass, obj1, is passed to a function useObject by value, thus making a copy of it.

Let us take an immediate look at the output of the program, before going into details:

```sh
(1)
CREATING instance of MyMovableClass at 0x7ffeefbff718 allocated with size = 400 bytes

(2)
COPYING content of instance 0x7ffeefbff718 to instance 0x7ffeefbff708

using object 0x7ffeefbff708

(3)
DELETING instance of MyMovableClass at 0x7ffeefbff708

(4)
CREATING instance of MyMovableClass at 0x7ffeefbff6d8 allocated with size = 800 bytes

(5)
MOVING (c'tor) instance 0x7ffeefbff6d8 to instance 0x7ffeefbff6e8

using object 0x7ffeefbff6e8

DELETING instance of MyMovableClass at 0x7ffeefbff6e8
DELETING instance of MyMovableClass at 0x7ffeefbff6d8
DELETING instance of MyMovableClass at 0x7ffeefbff718
```

First, we are creating an instance of MyMovableClass, obj1, by calling the constructor of the class (1).

Then, we are passing obj1 by-value to a function useObject, which causes a temporary object obj to be instantiated, which is a copy of obj1 (2) and is deleted immediately after the function scope is left (3).

Then, the function is called with a temporary instance of MyMovableClass as its argument, which creates a temporary instance of MyMovableClass as an rvalue (4). But instead of making a copy of it as before, the move constructor is used (5) to transfer ownership of that temporary object to the function scope, which saves us one expensive deep-copy.

---

### Moving lvalues

There is one final aspect we need to look at: In some cases, it can make sense to treat lvalues like rvalues. At some point in your code, you might want to transfer ownership of a resource to another part of your program as it is not needed anymore in the current scope. But instead of copying it, you want to just move it as we have seen before. The "problem" with our implementation of MyMovableClass is that the call useObject(obj1) will trigger the copy constructor as we have seen in one of the last examples. But in order to move it, we would have to pretend to the compiler that obj1 was an rvalue instead of an lvalue so that we can make an efficient move operation instead of an expensive copy.

There is a solution to this problem in C++, which is std::move. This function accepts an lvalue argument and returns it as an rvalue without triggering copy construction. So by passing an object to std::move we can force the compiler to use move semantics, either in the form of move constructor or the move assignment operator:

```c++
int main()
{
    MyMovableClass obj1(100); // constructor

    useObject(std::move(obj1));

    return 0;
}
```

Nothing much has changed, apart from obj1 being passed to the std::move function. The output would look like the following:

```sh
CREATING instance of MyMovableClass at 0x7ffeefbff718 allocated with size = 400 bytes

MOVING (c'tor) instance 0x7ffeefbff718 to instance 0x7ffeefbff708

using object 0x7ffeefbff708

DELETING instance of MyMovableClass at 0x7ffeefbff708
DELETING instance of MyMovableClass at 0x7ffeefbff718
```

By using std::move, we were able to pass the ownership of the resources within obj1 to the function useObject. The local copy obj1 in the argument list was created with the move constructor and thus accepted the ownership transfer from obj1 to obj . Note that after the call to useObject, the instance obj1 has been invalidated by setting its internal handle to null and thus may not be used anymore within the scope of main (even though you could theoretically try to access it, but this would be a really bad idea).

---
