# When to use raw pointers and smart pointers

As a general rule of thumb with modern C++, smart pointers should be used often. They will make your code safer as you no longer need to think (much) about the proper allocation and deallocation of memory. As a consequence, there will be much fewer memory leaks caused by dangling pointers or crashes from accessing invalidated memory blocks.

When using raw pointers on the other hand, your code might be susceptible to the following bugs:

1- Memory leaks
2- Freeing memory that shouldn’t be freed
3- Freeing memory incorrectly
4- Using memory that has not yet been allocated
5- Thinking that memory is still allocated after being freed

With all the advantages of smart pointers in modern C++, one could easily assume that it would be best to completely ban the use of new and delete from your code. However, while this is in many cases possible, it is not always advisable as well. Let us take a look at the C++ core guidelines, which has several rules for explicit memory allocation and deallocation. **We will briefly discuss three of them:**

**1- R. 10: Avoid malloc and free**
While the calls of `(MyClass*)malloc( sizeof(MyClass) )` and `new MyClass` both allocate a block of memory on the heap in a perfectly valid manner, only new will also call the constructor of the class and free the destructor. **To reduce the risk of undefined behavior, malloc and free should thus be avoided.**

**2- R. 11: Avoid calling new and delete explicitly**
Programmers have to make sure that every call of new is paired with the appropriate delete at the correct position so that no memory leak or invalid memory access occur. The emphasis here lies in the word "explicitly" as opposed to implicitly, such as with smart pointers or containers in the standard template library.

**3- R. 12: Immediately give the result of an explicit resource allocation to a manager object**
It is recommended to make use of manager objects for controlling resources such as files, memory or network connections to mitigate the risk of memory leaks. This is the core idea of smart pointers.

**Summarizing**
Raw pointers created with new and delete allow for a high degree of flexibility and control over the managed memory. To mitigate their proneness to errors, the following additional recommendations can be given:

- A call to new should not be located too far away from the corresponding delete. It is bad style to stretch you new / delete pairs throughout your program with references criss-crossing your entire code.

- Calls to new and delete should always be hidden from third parties so that they must not concern themselves with managing memory manually **(which is similar to R. 12)**.

In addition to the above recommendations, the C++ core guidelines also contain a total of 13 rules for the recommended use of smart pointers. In the following, we will discuss a selection of these:
**1- R. 20 : Use unique_ptr or shared_ptr to represent ownership**

**2- R. 21 : Prefer unique_ptr over std::shared_ptr unless you need to share ownership**
Both pointer types express ownership and responsibilities **(R. 20)**. A unique_ptr is an exclusive owner of the managed resource; therefore, it cannot be copied, only moved. In contrast, a shared_ptr shares the managed resource with others. As described above, this mechanism works by incrementing and decrementing a common reference counter. The resulting administration overhead makes shared_ptr more expensive than unique_ptr. For this reason unique_ptr should always be the first choice **(R. 21)**.

**3- R. 22 : Use `make_shared()` to make shared_ptr**

**4- R. 23 : Use `make_unique()` to make std::unique_ptr**

The increased management overhead compared to raw pointers becomes in particular true if a shared_ptr is used.

**Creating a shared_ptr requires**
(1) the allocation of the resource using new.
(2) the allocation and management of the reference counter.
Using the factory function **make_shared** is a one-step operation with lower overhead and should thus always be preferred. **(R.22)**. This also holds for unique_ptr **(R.23)**, although the performance gain in this case is minimal (if existent at all).

But there is an additional reason for using the make_... factory functions: Creating a smart pointer in a single step removes the risk of a memory leak. Imagine a scenario where an exception happens in the constructor of the resource. In such a case, the object would not be handled properly and its destructor would never be called - even if the managing object goes out of scope. Therefore, make_shared and make_unique should always be preferred.
**Note that make_unique is only available with compilers that support at least the C++14 standard**.

**Example to show the difference between the 2 approches:-**
1- `std::unique_ptr<int> uniquePtr(new int);`
This syntax directly uses the new keyword to allocate memory for an int and initializes a std::unique_ptr to manage it.
**Pros:**

- Direct and explicit.

**Cons:**

- **Exception Safety:** If an exception is thrown between the new int allocation and the uniquePtr initialization, the allocated memory will leak because the pointer will not be properly managed by the uniquePtr.

- **Verbose:** Requires more code and is prone to errors, especially with more complex types.

2- `std::unique_ptr<int> make_unique<new>();`
This syntax uses the std::make_unique function, introduced in C++14, to create a std::unique_ptr managing a newly allocated int.

**Pros:**

- **Exception Safety:** The memory allocation and unique_ptr initialization are done in a single step, so there is no risk of a memory leak if an exception is thrown.
- **Simplicity and Clarity:** The code is more concise and easier to read.
- **Performance:** The std::make_unique is often more efficient because it can use in-place construction, reducing the overhead of separate allocation and initialization steps.

**Cons:**

- Requires C++14 or later.

**In summary**, using std::make_unique is generally preferred because it provides better exception safety, more concise code, and can be more efficient.

**5- R. 24 : Use weak_ptr to break cycles of shared_ptr**
We have seen that weak pointers provide a way to break a deadlock caused by two owning references which are cyclicly referring to each other. With weak pointers, a resource can be safely deallocated as the reference counter is not increased.

The remaining set of guideline rules referring to smart pointers are mostly concerning the question of how to pass a smart pointer to a function.

---
