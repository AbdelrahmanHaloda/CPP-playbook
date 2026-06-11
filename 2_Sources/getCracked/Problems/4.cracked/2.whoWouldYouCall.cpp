// Who'd you call?

// Observe the code snippet below. What gets printed? Assume C++17 and beyond.

// Headers

struct A {
    A() { cout << 'a'; }
    A(const A &a) { cout << 'b'; }
    A(A &&a) noexcept { std::cout << 'c'; }
    A& operator=(const A &a) { cout << 'd'; return *this; }
    A& operator=(A && a) noexcept { cout << 'e'; return *this; }
    ~A() { cout << 'f'; }
};

A foo(A a) {
    a = A();
    cout << 'H';
    return std::move(a);
}

int main() {
    A *a = new A;
    foo(*a);    
                 

    if (a) {
        A b = A();
        A c;
        b = c;
    }

    cout << 'K';

    delete a;
}

/*
/home/ahaloda/REPOS/cpp/4.cracked/whoWouldYouCall.png
*/