// Who are you pointing to?

// Observe the code snippet below. What is its equivalent of a->foo()? Assume the variable is of some pointer type that has a foo method.

struct A { void foo() { } };

int main() {
    A* a = new A();
    a->foo();
}

/*
(*a).foo()

-> is just syntactic sugar for (*variable).method(). Without it, we'd have error prone code where we're constantly writing dereference operations. It can also be represented as A::foo(&a). Other questions explore its implications.
*/