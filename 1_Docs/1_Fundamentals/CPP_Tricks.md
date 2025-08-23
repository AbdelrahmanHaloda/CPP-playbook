## C++ Tips

### 1) auto Keyword

The auto keyword in C++ is evaluated at compile time, allowing the compiler to deduce the data type of a variable during compilation.

#### Caution when using auto with const:

```c++

int x = 5;

auto y = x; // `auto` deduces `int`, so `y` is `int`, not `const int`. This can be risky!

```
No such issue with auto and references:

```c++

int x = 5;

auto& y = x; // `auto` deduces `const int`, so `y` is a `const int&`, as expected since references are aliases.

```

### 2) delegation constructor

initalization of delegration constructor must appear alone.

```c++
class A
{
    public:
        A()
        {
            cout<<"Basic constructor">>endl;
        }
        /* valid case, delegated constructor call main constructor in initialization list alone */
        // A(int x, int b): A()
        // {
        //     cout<<"delegated constructor, valid"<<endl;
        // }
        /* Invalid case, delegated constructor call main constructor in in itialization list with other code */

        A(int x, int b): A(), x(x)
        {
            cout<<"delegated constructor, error !"<<endl;
        }

    private:
        int x;
        int y;
};

```

```output

error: an initializer for a delegating constructor must appear alone
   17 |         A(int x, int b): A(), x(x)
      |                          ^~~  ~~~~
1 error generated.
```

---

### 3)
