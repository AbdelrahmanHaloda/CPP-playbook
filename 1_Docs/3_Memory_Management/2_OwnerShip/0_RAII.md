# RAII (Resource Acquisition is Initialization)

The **RAII** is a widespread programming paradigm, that can be used to protect a resource such as a file stream, a network connection or a block of memory which need proper management.

**Acquiring and releasing resources**
In most programs of reasonable size, there will be many situations where a certain action at some point will necessitate a proper reaction at another point, such as: **[Files, Memory, Locks]**

- Allocating memory with new or malloc, which must be matched with a call to delete or free.

- Opening a file or network connection, which must be closed again after the content has been read or written.

- Protecting synchronization primitives such as atomic operations, memory barriers, monitors or critical sections, which must be released to allow other threads to obtain them.

The following table gives a brief overview of some resources and their respective allocation and deallocation calls in C++:
[Resource able](/Users/abdelrahmanhaloda/AHossam/REPOS/CPP-playbook/1_Docs/3_Memory_Management/Images/RAII.png)

---

## RAII to the rescue

The major idea of RAII revolves around object ownership and information hiding: Allocation and deallocation are hidden within the management class, so a programmer using the class does not have to worry about memory management responsibilities. If he has not directly allocated a resource, he will not need to directly deallocate it - whoever owns a resource deals with it. In the case of RAII this is the management class around the protected resource. The overall goal is to have allocation and deallocation (e.g. with new and delete) disappear from the surface level of the code you write.

RAII can be used to leverage - among others - the following advantages:

- Use class destructors to perform resource clean-up tasks such as proper memory deallocation when the RAII object gets out of scope

- Manage ownership and lifetime of dynamically allocated objects

- Implement encapsulation and information hiding due to resource acquisition and release being performed within the same object.

In the following, let us look at RAII from the perspective of memory management. There are three major parts to an RAII class:

- A resource is allocated in the constructor of the RAII class
- The resource is deallocated in the destructor
- All instances of the RAII class are allocated on the stack to reliably control the lifetime via the object scope

---
