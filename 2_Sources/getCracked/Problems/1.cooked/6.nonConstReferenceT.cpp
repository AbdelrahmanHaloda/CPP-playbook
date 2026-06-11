// Non-const reference T

// Observe the code snippet below carefully. What is the value of i after the call?

template <typename T>
requires std::is_integral_v<T>
void foo(T& p) {
    p = 1;
}

int main() {
    const int i = 0;
    foo(i);
}

/*
Compilation error

What's being tested here is whether a non-const T& will or will not adopt the constness of its parameter. If you pass a const arguent, the deduction will result in the argument becoming a declaration of a const reference. Passing a const object is possible in the call expression itself, but when the function is fully instantiated (which happens later in the copilation process) any attept to modify the value will trigger an error. This concept will be further explored in another question.
*/