# Template Specialization

## 1. Introduction

Template specialization allows you to **customize behavior of templates based on types or values**.

> Same template → different behavior depending on template arguments

This is one of the core mechanisms behind:
- STL type traits
- Optimized implementations
- Compile-time decision making

---

## 2. The Problem Specialization Solves

Generic templates assume all types behave the same:

```cpp
template <typename T>
void print(T value) {
    std::cout << value;
}
```

But sometimes we need:

- Different formatting
- Different algorithms
- Different performance paths

Example:

```cpp
print(10);        // OK
print("hello");  // Might need different handling
```

---

### Key Insight

> Specialization exists to override generic behavior when a type or value requires different logic

---

## 3. Full Specialization (Exact Match)

Full specialization targets **one exact type**.

### Syntax

```cpp
template <typename T>
struct Printer {
    static void print(T value) {
        std::cout << value;
    }
};

// Full specialization
template <>
struct Printer<const char*> {
    static void print(const char* value) {
        std::cout << "string: " << value;
    }
};
```

### Behavior

```cpp
Printer<int>::print(10);            // generic
Printer<const char*>::print("hi"); // specialized
```

---

### Mental Model

> Full specialization = exact override for one specific type

---

### Real-Life Use Case

- Special handling for strings
- Custom behavior for specific containers
- Optimized paths for known types

---

## 4. Partial Specialization (Pattern Matching)

Partial specialization targets a **family of types using patterns**.

### Syntax

```cpp
template <typename T>
struct Traits {
    static constexpr bool is_pointer = false;
};

// Partial specialization
template <typename T>
struct Traits<T*> {
    static constexpr bool is_pointer = true;
};
```

### Behavior

```cpp
Traits<int>::is_pointer;      // false
Traits<int*>::is_pointer;     // true
Traits<double*>::is_pointer;  // true
```

---

### Mental Model

> Partial specialization = pattern-based override (type matching)

---

### Real-Life Use Case

- Detecting pointers
- Detecting containers
- Type traits (core STL usage)

---

## 5. Specialization with Non-Type Parameters (Values)

Templates can also be specialized based on **values**.

### Syntax

```cpp
template <int N>
struct Buffer {
    static constexpr int size = N;
};

// Special case
template <>
struct Buffer<0> {
    static constexpr int size = -1;
};
```

---

### Mental Model

> Values can control behavior at compile time

---

### Real-Life Use Case

- Special handling for edge cases (size = 0)
- Compile-time configuration
- Fixed-size containers

---

## 6. Function Templates (Important Rule)

Function templates:
- ❌ Cannot be partially specialized
- ✅ Can be overloaded
- ✅ Can be fully specialized (rare)

### Preferred Approach (Overloading)

```cpp
template <typename T>
void print(T value) {
    std::cout << value;
}

void print(const char* value) {
    std::cout << "string: " << value;
}
```

---

### Mental Model

> For functions → use overloading instead of specialization

---

## 7. How the Compiler Chooses

When multiple options exist, the compiler selects:

1. Most specialized match
2. Then less specialized
3. Then generic template

### Example

```cpp
template <typename T>
struct X {};

template <typename T>
struct X<T*> {};

template <>
struct X<int*> {};
```

### Resolution

```cpp
X<double*> → uses X<T*>
X<int*>    → uses X<int*>
X<int>     → uses generic
```

---

### Mental Model

> Compiler picks the most specific match

---

## 8. Comparison Table

| Feature | Full Specialization | Partial Specialization |
|--------|--------------------|------------------------|
| Match type | Exact type | Pattern of types |
| Flexibility | Low | High |
| Example | `X<int*>` | `X<T*>` |
| Use case | Special case | Type families |

---

## 9. Real-World Examples

### Example 1: Type Trait

```cpp
template <typename T>
struct IsPointer {
    static constexpr bool value = false;
};

// partial specialization
template <typename T>
struct IsPointer<T*> {
    static constexpr bool value = true;
};
```

---

### Example 2: Optimized Implementation

```cpp
template <typename T>
struct Copier {
    static void copy(T* dst, const T* src) {
        // generic copy
    }
};

// specialization for char
template <>
struct Copier<char> {
    static void copy(char* dst, const char* src) {
        std::memcpy(dst, src, strlen(src));
    }
};
```

---

## 10. Common Pitfalls

### 10.1 Exact Match Confusion

```cpp
Printer<const char*> ≠ Printer<char*>
```

---

### 10.2 Visibility Issue

Specialization must be visible before use.

---

### 10.3 Function vs Class Confusion

- Class templates → support partial specialization
- Function templates → use overloading

---

## 11. Mental Model Summary

> Specialization = compile-time decision system

- Generic → default behavior
- Partial → pattern-based override
- Full → exact override

---

## 12. Interview Questions (Advanced)

### 12.1 What is template specialization?
Customization of template behavior for specific types or values at compile time.

---

### 12.2 Full vs Partial specialization?
Full → exact type, Partial → pattern of types.

---

### 12.3 Why can't function templates be partially specialized?
Because C++ uses overload resolution instead.

---

### 12.4 Which one is selected?

```cpp
template <typename T> struct X {};
template <typename T> struct X<T*> {};
template <> struct X<int*> {};
```

Answer:
- `X<int*>` → full specialization
- `X<double*>` → partial specialization
- `X<int>` → generic

---

### 12.5 Trap Question

```cpp
template <typename T>
struct X<T*> {};
```

Why is this partial and not full?

Answer:
Because `T` is still unknown → not fully specified.

---

### 12.6 Trick Question

Why might specialization not be used?

Answer:
- Not visible at instantiation
- Type mismatch

---

### 12.7 Design Question

When should you use specialization?

Answer:
When a type or category of types requires different behavior than the generic implementation.

---

## 13. Final Mental Anchor

- Templates generate code
- Specialization customizes behavior
- Compiler selects the most specific match
- Partial = pattern, Full = exact