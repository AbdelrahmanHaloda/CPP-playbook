// Observe the code snippet below, which assignment will cause a compile error? Assume C++17 or greater.

void fNoexcept() noexcept;
void fMightThrow();

void (*fp)() noexcept;
void (*fp2)();

fp = fNoexcept; // 1
fp = fMightThrow; // 2

fp2 = fNoexcept; // 3
fp2 = fMightThrow; // 4

/*
The compile error will occur on assignment 2 because a function pointer declared with noexcept (like fp) represents a promise that the function being called will not throw an exception; 

assigning a potentially throwing function (like fMightThrow) to it would break that promise, as the caller might rely on the noexcept guarantee for safety or optimization. 

The reverse assignment (line 3) is safe because a noexcept function is a stricter subset (calling it through a pointer without the noexcept annotation is still valid since it won't throw, even though the pointer type allows the possibility).

This rule was added in C++17 to enforce exception safety at the type system level.
*/

---
