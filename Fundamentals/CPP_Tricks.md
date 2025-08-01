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

---