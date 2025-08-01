### Heap Memory
#### Properties of Heap Memory
Heap memory, also know as dynamic memory , is an important resource available to programs (and programmers) to store data. The following diagram again shows the layout of virtual memory with the heap being right above the BSS and Data segment.

The heap memory grows upwards while the stack grows in the opposite direction. The automatic stack memory shrinks and grows with each function call and local variable. As soon as the scope of a variable is left, it is automatically deallocated and the stack pointer is shifted upwards accordingly.

Heap memory is different in many ways: The programmer can request the allocation of memory by issuing a command such as malloc or new. This block of memory will remain allocated until the programmer explicitly issues a command such as free or delete. The huge advantage of heap memory is the high degree of control a programmer can exert, albeit at the price of greater responsibility since memory on the heap must be actively managed.

Let us take a look at some properties of heap memory:

1- As opposed to local variables on the stack, memory can now be allocated in an arbitrary scope (e.g. inside a function) without it being deleted when the scope is left. Thus, as long as the address to an allocated block of memory is returned by a function, the caller can freely use it.

2- Local variables on the stack are allocated at compile-time. Thus, the size of e.g. a string variable might not be appropriate as the length of the string will not be known until the program is executed and the user inputs it. With local variables, a solution would be to allocate a long-enough array of and hope that the actual length does not exceed the buffer size. With dynamically allocated heap memory, variables are allocated at run-time. This means that the size of the above-mentioned string variable can be tailored to the actual length of the user input.

3- Heap memory is only constrained by the size of the address space and by the available memory. With modern 64 bit operating systems and large RAM memory and hard disks the programmer commands a vast amount of memory. However, if the programmer forgets to deallocate a block of heap memory, it will remain unused until the program is terminated. This is called a "memory leak".

4- Unlike the stack, the heap is shared among multiple threads, which means that memory management for the heap needs to take concurrency into account as several threads might compete for the same memory resource.

5- When memory is allocated or deallocated on the stack, the stack pointer is simply shifted upwards or downwards. Due to the sequential structure of stack memory management, stack memory can be managed (by the operating system) easily and securely. With heap memory, allocation and deallocation can occur arbitrarily, depending on the lifetime of the variables. This can result in fragmented memory over time, which is much more difficult and expensive to manage.


#### Memory Fragmentation
Let us construct a theoretic example of how memory on the heap can become fragmented: Suppose we are interleaving the allocation of two data types X and Y. First, we allocate a block of memory for a variable of type X, then another block for Y and so on in a repeated manner until some upper bound is reached.

At some point, we might then decide to deallocate all variables of type Y, leading to empty spaces in between the remaining variables of type X. In between two blocks of type "X", no memory for an additional "X" could now be squeezed in this example.

Image showing memory with empty spaces between variables of type x.
A classic symptom of memory fragmentation is that you try to allocate a large block and you can’t, even though you appear to have enough memory free. On systems with virtual memory however, this is less of a problem, because large allocations only need to be contiguous in virtual address space, not in physical address space.

When memory is heavily fragmented however, memory allocations will likely take longer because the memory allocator has to do more work to find a suitable space for the new object.

Until now, our examples have been only theoretical. It is time to gain some practical experience in the next section using malloc and free as C-style methods for dynamic memory management.

---
### Allocating Dynamic Memory
#### Using malloc, calloc, free
To allocate dynamic memory on the heap means to make a contiguous memory area accessible to the program at runtime and to mark this memory as occupied so that no one else can write there by mistake.

To reserve memory on the heap, one of the two functions malloc (stands for Memory Allocation) or calloc (stands for Cleared Memory Allocation) is used. The header file stdlib.h or malloc.h must be included to use the functions.

Here is the syntax of malloc and calloc in C/C++:

```
pointer_name = (cast-type*) malloc(size);
pointer_name = (cast-type*) calloc(num_elems, size_elem);
```
**malloc** is used to dynamically allocate a single large block of memory with the specified size. It returns a pointer of type void which can be cast into a pointer of any form.

**calloc** is used to dynamically allocate the specified number of blocks of memory of the specified type. It initializes each block with a default value '0'.

Both functions return a pointer of type void which can be cast into a pointer of any form. If the space for the allocation is insufficient, a NULL pointer is returned.

**NOTE:-**
The return addess of malloc or calloc must be casted to a defined datatype otherwise, a compilation error shall be generated.
Because the problem with void pointers is that there is no way of knowing the offset to the end of the allocated memory block. For an int, this would be 4 bytes but for a double, the offset would be 8 bytes. So in order to retrieve the entire block of memory that has been reserved, we need to know the data type and the way to achieve this is by casting the return pointer: like that `int *p = (int*)malloc(sizeof(int));`

The size of the memory area reserved with malloc or calloc can be increased or decreased with the **realloc function**.

`pointer_name = (cast-type*) realloc( (cast-type*)old_memblock, new_size );`
` p = (int*)realloc(p,2*sizeof(int));`

#### Freeing up Memory
If memory has been reserved, it should also be released as soon as it is no longer needed. If memory is reserved regularly without releasing it again, the memory capacity may be exhausted at some point. If the RAM memory is completely used up, the data is swapped out to the hard disk, which slows down the computer significantly.

The free function releases the reserved memory area so that it can be used again or made available to other programs. To do this, the pointer pointing to the memory area to be freed is specified as a parameter for the function.
```
void *p = malloc(100); 
free(p);
```
**NOTE:-**
1- free can only free memory that was reserved with malloc or calloc.

2- free can only release memory that has not been released before. Releasing the same block of memory twice will result in an error.

---
### Using new and delete
#### Comparing malloc with new
The functions malloc and free are library function and represent the default way of allocating and deallocating memory in C. In C++, they are also part of the standard and can be used to allocate blocks of memory on the heap.

With the introduction of classes and object oriented programming in C++ however, memory allocation and deallocation has become more complex: When an object is created, its constructor needs to be called to allow for member initialization. Also, on object deletion, the destructor is called to free resources and to allow for programmer-defined clean-up tasks. For this reason, C++ introduces the operators new / delete, which represent the object-oriented counterpart to memory management with malloc / free.

If we were to create a C++ object with malloc, the constructor and destructor of such an object would not be called. Consider the following example

In main, we will allocate memory for an instance of MyClass using both malloc/ free and new/delete.

With malloc, the program crashes on calling the method setNumber, as no memory has been allocated for _number - because the constructor has not been called. Hence, an EXC_BAD_ACCESS error occurs, when trying to access the memory location to which _number is pointing. With _new, the output looks like the following:
```
Allocate memory
Number: 42
Delete memory
```

```
#include <stdlib.h>
#include <iostream>

class MyClass
{
private:
    int *_number;

public:
    MyClass()
    {
        std::cout << "Allocate memory\n";
        _number = (int *)malloc(sizeof(int));
    }
    ~MyClass()
    {
        std::cout << "Delete memory\n";
        free(_number);
    }
    void setNumber(int number)
    {
        *_number = number;
        std::cout << "Number: " << * _number << "\n";
    }
};

int main()
{
    /* allocate memory using malloc */
    /* comment these lines out to run the example below */
    MyClass *myClass = (MyClass *)malloc(sizeof(MyClass));
    myClass->setNumber(42); // EXC_BAD_ACCESS
    free(myClass);
	  
 	// allocate memory using new
    // MyClass *myClass = new MyClass();
    // myClass->setNumber(42); // works as expected
    // delete myClass;

    return 0;
}
```
#### Summarize the major differences between malloc/free and new/delete:

1- Constructors / Destructors Unlike malloc( sizeof(MyClass) ), the call new MyClass() calls the constructor. Similarly, delete calls the destructor.

2- Type safety malloc returns a void pointer, which needs to be cast into the appropriate data type it points to. This is not type safe, as you can freely vary the pointer type without any warnings or errors from the compiler as in the following small example: MyObject *p = (MyObject*)malloc(sizeof(int));

In C++, the call `MyObject *p = new MyObject()` returns the correct type automatically - it is thus type-safe.

#### Example on new and delete overloading
By customizing the new operator to use malloc in allocating memory, this way we managed to use the full functionality of new which includes calling the constructor and also we explicitly handled the memory allocation by using malloc
```
#include <iostream>
#include <stdlib.h>

class MyClass
{
    int _mymember;

public:
    MyClass()
    {
        std::cout << "Constructor is called\n";
    }

    ~MyClass()
    {
        std::cout << "Destructor is called\n";
    }

    void *operator new(size_t size)
    {
        std::cout << "new: Allocating " << size << " bytes of memory" << std::endl;
        void *p = malloc(size);

        return p;
    }

    void operator delete(void *p)
    {
        std::cout << "delete: Memory is freed again " << std::endl;
        free(p);
    }
};

int main()
{
    MyClass *p = new MyClass();
    delete p;
}
```

```
// output
new: Allocating 4 bytes of memory
Constructor is called
Destructor is called
delete: Memory is freed again 
```
As can be seen from the order of text output, memory is instantiated in new before the constructor is called, while the order is reversed for the destructor and the call to delete.
#### Summary
1- Using malloc: Allocates memory but does not call the constructor, resulting in an uninitialized object.
2- Using new: Allocates memory and calls the constructor, resulting in a properly initialized object.
3- Overloaded new: Customizes the memory allocation but still ensures that the constructor is called, as per the C++ object creation process.

#### Reasons for overloading new and delete
1- The overloaded new operator function allows to add additional parameters. Therefore, a class can have multiple overloaded new operator functions. This gives the programmer more flexibility in customizing the memory allocation for objects.
2- Overloading the new and delete operators provides an easy way to integrate a mechanism similar to garbage collection capabilities (such as in Java).
3- By adding exception handling capabilities into new and delete, the code can be made more robust.
4- It is very easy to add customized behavior, such as overwriting deallocated memory with zeros in order to increase the security of critical application data.

---

### Bjarne on new and delete
Rule of thumb is don't use new and delete should in the apllication code. new and delete belongs in the implementation of your abstractions. So if you use new a vector of strings,  you don't use new and delete, it's gone. it's hidden inside the abstraction. 

---

### The worst problems with new and delete:

- **Proper pairing of new and delete :** Every dynamically allocated object that is created with new must be followed by a manual deallocation at a "proper" place in the program. If the programer forgets to call delete (which can happen very quickly) or if it is done at an "inappropriate" position, memory leaks will occur which might clog up a large portion of memory.

- **Correct operator pairing :** C++ offers a variety of new/delete operators, especially when dealing with arrays on the heap. A dynamically allocated array initialized with new[] may only be deleted with the operator delete[]. If the wrong operator is used, program behavior will be undefined - which is to be avoided at all cost in C++.

- **Memory ownership :** If a third-party function returns a pointer to a data structure, the only way of knowing who will be responsible for resource deallocation is by looking into either the code or the documentation. If both are not available (as is often the case), there is no way to infer the ownership from the return type. As an example, in the final project of this course, we will use the graphical library wxWidgets to create the user interface of a chatbot application. In wxWidgets, the programmer can create child windows and control elements on the heap using new, but the framework will take care of deletion altogether. If for some reason the programmer does not know this, he or she might call delete and thus interfere with the inner workings of the wxWidgets library.

---