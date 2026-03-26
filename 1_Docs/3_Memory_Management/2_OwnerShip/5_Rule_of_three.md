
# The Rule of Three

In the previous examples we have taken a first look at several copying policies:

- Default copying
- No copying
- Exclusive ownership
- Deep copying
- Shared ownership

In the first example we have seen that the default implementation of the copy constructor does not consider the "special" needs of a class which allocates and deallocates a shared resource on the heap. The problem with implicitly using the default copy constructor or assignment operator is that programmers are not forced to consider the implications for the memory management policy of their program. In the case of the first example, this leads to a segmentation fault and thus a program crash.

In order to properly manage memory allocation, deallocation and copying behavior, we have seen that there is an intricate relationship between **destructor**, **copy constructor** and **copy assignment operator**. To this end, **the Rule of Three** states that if a class needs to have an overloaded **copy constructor**, **copy assignment operator**, or **destructor**, then it must also implement the other two as well to ensure that memory is managed consistently. As we have seen, the copy constructor and copy assignment operator (which are often almost identical) control how the resource gets copied between objects while the destructor manages the resource deletion.

You may have noted that in the previous code example, the class SharedCopy does not implement the assignment operator. This is a violation of the Rule of Three and thus, if we were to use something like destination3 = source instead of SharedCopy destination3(source), the counter variable would not be properly decremented.

The copying policies discussed are the basis for a powerful concept in C++11 - smart pointers. But before we discuss these, we need to go into further detail on **move semantics**, which is a prerequisite you need to learn more about so you can properly understand the exclusive ownership policy as well as **the Rule of Five**, But before we discuss move semantics, we need to look into the concept of lvalues and rvalues in the next section.

---
