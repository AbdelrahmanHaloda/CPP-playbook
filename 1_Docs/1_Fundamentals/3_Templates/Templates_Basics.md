# Templates — Basics

## 1. What are Templates?

Templates are a C++ feature for **generic programming**, allowing you to write a single function or class that works with multiple data types. Instead of duplicating code for different types, you define a generic version and the compiler generates the specific versions needed at compile time.

This provides **compile-time polymorphism**, offering the flexibility of a single implementation for many types with the performance of type-specific code, all with zero runtime overhead.

**Why it matters:** Templates are the backbone of the Standard Template Library (STL), used in `std::vector`, `std::sort`, `std::shared_ptr`, and many other high-performance, reusable components.

---

## 2. Function Templates

A function template is a blueprint for creating functions.

### Basic Example
Instead of writing separate `sum` functions for `int` and `double`:
```cpp
// Overloads for specific types
int sum(int a, int b) { return a + b; }
double sum(double a, double b) { return a + b; }
```

You can write one template:
```cpp
template<typename T>
T sum(T a, T b) {
    return a + b;
}
```
Here, `T` is a **template parameter** that represents a type.

### How to Call a Function Template
**1. Type Deduction:** The compiler automatically determines the type from the arguments.
```cpp
sum(20, 13);          // Compiler deduces T = int
sum(20.0, 13.7);      // Compiler deduces T = double
```
**Note:** Type deduction is strict. `sum(1, 2.5)` would fail because the compiler can't deduce a single type `T`.

**2. Explicit Instantiation:** You can explicitly specify the type.
```cpp
sum<double>(20.0, 13.7);
sum<char>('Z', 'j');
```

---

## 3. Class Templates

Class templates allow you to create generic classes, like containers that can hold any data type.

### Example: A Generic `Box`
```cpp
template<typename T>
class Box {
private:
    T value;
public:
    void set(T val) { value = val; }
    T get() const { return value; }
};
```

### How to Use a Class Template
Unlike function templates, you must always explicitly specify the type for a class template when you create an object.

```cpp
Box<int> intBox;
intBox.set(123);

Box<std::string> stringBox;
stringBox.set("Hello");
```
`std::vector` is a classic example of a class template.

Since C++17, **Class Template Argument Deduction (CTAD)** can sometimes deduce the type from constructor arguments, but explicit is often clearer.

---

## 4. How Templates Work: Instantiation

Templates are not code themselves; they are **blueprints**. The compiler generates actual C++ code from these blueprints in a process called **instantiation**.

When you use a template like `sum<int>(1, 2)`, the compiler generates a specific version of that function for `int`. This happens at **compile-time**.

Your code:
```cpp
int main() {
    sum(5, 10);
    sum(3.14, 1.59);
}
```

What the compiler generates:
```cpp
int sum(int a, int b) { return a + b; }
double sum(double a, double b) { return a + b; }

int main() {
    sum(5, 10);
    sum(3.14, 1.59);
}
```

**Key takeaway:** Because this happens during compilation, there is **zero runtime overhead**.

---

## 5. Important Considerations

### Template Definitions Belong in Header Files
The compiler needs the full template definition (the blueprint) to generate code. If you define a template in a `.cpp` file, you will get linker errors because the definition won't be available in other files that use it. **Always define templates in header files (`.h` or `.hpp`).**

### Compile-Time vs. Runtime Polymorphism

| Feature | Templates (Compile-Time) | Virtual Functions (Runtime) |
|---|---|---|
| **Mechanism** | Code generation at compile time | Dynamic dispatch via vtable at runtime |
| **Performance** | Faster, no runtime overhead | Slower due to vtable lookup |
| **Flexibility** | Types must be known at compile time | Objects can be decided at runtime |
| **Use Case** | Generic algorithms, data structures | Handling objects of different types through a common interface |

---

## 6. Summary

- **Generic Code:** Templates let you write code that is independent of types.
- **Compile-Time Polymorphism:** They provide polymorphism without any runtime cost.
- **Instantiation:** The compiler generates type-specific code from your template blueprints.
- **Header-Only:** Template definitions must be in header files.
