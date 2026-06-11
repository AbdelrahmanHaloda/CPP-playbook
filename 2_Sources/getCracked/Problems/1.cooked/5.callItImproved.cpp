// Call it, improved.

// It is highly recommended to complete the previous problem titled 'Call it.' before attempting this one. 
// 
// Your co-worker wants to adjust the custom foreach algorithm to support another form of a callable, that being a non-static member function 
// (which doesn't match the pattern of a function-object). 
// Which utility, introduced in C++17, allows you to accomodate this requirement? The code snippet below shows how the custom foreach is called.

// Headers

struct Getcracked {
    void memfunc(int) const { std::cout << "Hi there."; }
};

template <typename Iter, typename Callable, typename... Args>
void foreach (Iter current, Iter end, Callable op, const Args&... args);
// Implementation of improved foreach goes here.

int main() {
    std::vector<int> primes = { 2, 3, 5, 7, 11 };
    Getcracked obj;
    foreach (primes.begin(), primes.end(), &Getcracked::memfunc, obj);
}

/*
std::invoke

You can view a sample implementation using std::invoke here (https://godbolt.org/z/9cGsq9K5c). 

Here we pass additional parameters along with the callable. 

std::invoke applies to the referenced element, but not before consuming additional arguments. std::invoke handles them as follows:

(i) if the callable is a pointer to member, it uses the first additional argument as the this object. All remaining additional parameters are just passed as arguments to the callable.

(ii) otherwise, all additional parameters are just passed as arguments to the callable. This version of foreach is superior to the previous version, since it can handle callables and pointers to member functions, including aditional arguments.
*/