### The Unique pointer
A unique pointer is the exclusive owner of the memory resource it represents. There must not be a second unique pointer to the same memory resource, otherwise there will be a compiler error. As soon as the unique pointer goes out of scope, the memory resource is deallocated again. Unique pointers are useful when working with a temporary heap resource that is no longer needed once it goes out of scope.

`std::unique_ptr<Type> p(new Type);`

```
void RawPointer()
{
    int *raw = new int; // create a raw pointer on the heap
    *raw = 1; // assign a value
    delete raw; // delete the resource again
}

void UniquePointer()
{
    std::unique_ptr<int> unique(new int); // create a unique pointer on the stack
    *unique = 2; // assign a value
    // delete is not neccessary
}
```
**NOTE**
Note that smart pointers always need to be declared on the stack, otherwise the scoping mechanism would not work.

The smart pointer destructor contains the call to delete, and because the smart pointer is declared on the stack, its destructor is invoked when the smart pointer goes out of scope, even if an exception is thrown.

Example on Unique pointer

```
#include <iostream>
#include <memory>
#include <string>

class MyClass
{
private:
    std::string _text;

public:
    MyClass() {}
    MyClass(std::string text) { _text = text; }
    ~MyClass() { std::cout << _text << " destroyed" << std::endl; }
    void setText(std::string text) { _text = text; }
};

int main()
{
    // create unique pointer to proprietary class
    std::unique_ptr<MyClass> myClass1(new MyClass());
    std::unique_ptr<MyClass> myClass2(new MyClass("String 2"));

    // call member function using ->
    myClass1->setText("String 1");

    // use the dereference operator * 
    *myClass1 = *myClass2;

    // use the .get() function to retrieve a raw pointer to the object
    std::cout << "Objects have stack addresses " << myClass1.get() << " and " << myClass2.get() << std::endl;

    return 0;
}
```

In the example above, we will construct a unique pointer to a custom class. Also, we will see how the standard -> and * operators can be used to access member functions of the managed object, just as we would with a raw pointer:

Note that the custom class MyClass has two constructors, one without arguments and one with a string to be passed, which initializes a member variable _text that lives on the stack. Also, once an object of this class gets destroyed, a message to the console is printed, along with the value of _text. In main, two unique pointers are created with the address of a MyClass object on the heap as arguments. With myClass2, we can see that constructor arguments can be passed just as we would with raw pointers. After both pointers have been created, we can use the -> operator to access members of the class, such as calling the function setText. From looking at the function call alone you would not be able to tell that myClass1 is in fact a smart pointer. Also, we can use the dereference operator * to access the value of myClass1 and myClass2 and assign the one to the other. Finally, the . operator gives us access to proprietary functions of the smart pointer, such as retrieving the internal raw pointer with get().

**Summary**
- `&uniquePtr` --> the addres of the smartPtr in the stack
- `uniquePtr.get()` --> the address of the object on heap in which the uniquePtr is allocated.
- `uniquePtr.` --> is used to access members/methods of the uniquePtr object
- `uniquePt->`  --> is used to access members/methods of the object allocated by the uniquePtr
- `*uniquePtr`  --> is used to access members/methods of the object allocated by the uniquePtr howver a `.` must be used with it or alternatively u can use just `->`
such as `(*uniquePtr1) = (*uniquePtr2)` or `(*uniquePtr).method` or `uniquePt->method`


### Summing up
The unique pointer allows a single owner of the underlying internal raw pointer. Unique pointers should be the default choice unless you know for certain that sharing is required at a later stage. We have already seen how to transfer ownership of a resource using the Rule of Five and move semantics. Internally, the unique pointer uses this very concept along with RAII to encapsulate a resource (the raw pointer) and transfer it between pointer objects when either the move assignment operator or the move constructor are called. Also, a key feature of a unique pointer, which makes it so well-suited as a return type for many functions, is the possibility to convert it to a shared pointer. We will have a deeper look into this in the section on ownership transfer.

---

### unique_ptr mthodes

1- `.get()`
returns the raw pointer 
`*(unique_ptr.get())` is equivalent to `*unique_ptr`
