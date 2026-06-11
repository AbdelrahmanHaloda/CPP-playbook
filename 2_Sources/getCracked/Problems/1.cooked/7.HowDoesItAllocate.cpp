// How does it allocate?

// Observe the code snippet below, what is printed?

// Headers

struct Bar
{
    Bar() { std::cout << "1"; }
    Bar(const Bar&) { std::cout << "2"; }
};

int main()
{
    std::vector<Bar> bar(5);
}

/*
11111

Here we are invoking vector's constructor with n default-inserted elements. Default inserted elements call T's (Bar's) default constructor. Here is an example: explicit vector( size_type count, const T& value = T(), const Allocator& alloc = Allocator() );
*/