// 96% of you will fail this.

// If each of the lines in the main branch were executed in isolation, what would the output of the program be?

// Headers

class obj
{
public:
    obj() { std::cout << 1; }
    obj(const obj&) { std::cout << 2; }
    void operator=(const obj&) { std::cout << 3; }
};

int main()
{
    obj a;
    obj b{};
    obj c();
    obj d = a;
    d = b;
    auto e = obj{};
    obj f{a};
    obj g(a);
    auto h = b;
    obj(i);
}

/*
112312221

Here is a per-variable explanation: 
a: default constructor is called. 
b: brace-initailization, default constructor is called. 
c: C++'s most vexing parse. a warning is emitted, nothing is printed. 
d: This looks like copy assignment construction, but it's just copy construction since copy assignment only happens to an object already constructed. 
d: Copy assignment for the reason above. 
e: A more verbose way of default construction. 
f: Copy construction with brace-initialization. 
g: Copy construction with parentheses. 
h. Copy construction with the type deduced by auto. 
i. This is equivalent to obj i.
*/