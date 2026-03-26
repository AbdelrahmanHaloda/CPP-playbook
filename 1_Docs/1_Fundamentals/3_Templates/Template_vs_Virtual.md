

# Templates vs Virtual Functions

## 1. Introduction

C++ provides two main ways to achieve polymorphism:

- **Templates** → Compile-time polymorphism  
- **Virtual functions** → Runtime polymorphism  

Both solve similar problems (flexibility and reuse), but they operate at completely different stages of the program.

---

## 2. Core Idea

### Templates

```cpp
template<typename T>
void process(T value) {
    value.doSomething();
}
```

- Type is known at **compile time**
- Compiler generates **specific code per type**

---

### Virtual Functions

```cpp
class Base {
public:
    virtual void process() = 0;
};

class Derived : public Base {
public:
    void process() override {}
};
```

- Type is resolved at **runtime**
- Uses **dynamic dispatch (vtable)**

---

### Key Insight

> Templates decide behavior at **compile time**  
> Virtual functions decide behavior at **runtime**

### Quick Check

**Q: What is the main difference between templates and virtual functions?**  
A: Templates use compile-time polymorphism, while virtual functions use runtime polymorphism.

---

## 3. How They Work

### Templates (Compile-Time)

```cpp
process<int>(10);
process<double>(3.14);
```

Compiler generates:

```cpp
void process(int value);
void process(double value);
```

No runtime decision.

---

### Virtual Functions (Runtime)

```cpp
Base* obj = new Derived();
obj->process();
```

At runtime:
- Program checks vtable
- Calls correct function

---

### Key Insight

> Templates = code generation  
> Virtual = function selection

### Quick Check

**Q: When does the decision happen for templates vs virtual functions?**  
A: Templates → compile time, Virtual → runtime.

---

## 4. Performance

### Templates

- No indirection
- Fully optimizable
- Inlining possible

```cpp
process(10); // direct call
```

---

### Virtual Functions

- Indirection via pointer
- Cannot inline easily
- Slight runtime cost

```cpp
obj->process(); // indirect call via vtable
```

---

### Key Insight

> Templates have **zero runtime overhead**  
> Virtual functions have **small runtime cost**

### Quick Check

**Q: Why are templates faster than virtual functions?**  
A: Because they avoid runtime dispatch and allow full compiler optimization.

---

## 5. Flexibility

### Templates

✔ Work with **any type**  
❌ Type must be known at compile time  

---

### Virtual Functions

✔ Work with **unknown types at runtime**  
✔ Enable polymorphic containers  

---

### Example

```cpp
std::vector<Base*> objects;
```

You can store different derived types in the same container because they share a common base type.

### How this works (Runtime Polymorphism)

```cpp
class Base {
public:
    virtual void process() = 0;
};

class A : public Base {
public:
    void process() override {}
};

class B : public Base {
public:
    void process() override {}
};

std::vector<Base*> objects;
objects.push_back(new A());
objects.push_back(new B());

for (auto obj : objects)
    obj->process();  // resolved at runtime
```

- All objects are treated as `Base*`
- The actual function is selected **at runtime** using the vtable
- This allows **heterogeneous collections** (different types in the same container)

---

### Why Templates Cannot Do This

Templates require the type to be known at compile time.

```cpp
std::vector<A> v;   // only A
std::vector<B> v;   // only B
```

Each instantiation creates a completely different type:

```cpp
std::vector<A>  ≠  std::vector<B>
```

So this is not possible:

```cpp
std::vector<A or B>   // ❌ invalid
```

---

### Key Insight


### Quick Check

**Q: Which of the following is valid and why?**

```cpp
std::vector<Base*> v;
v.push_back(new A());
v.push_back(new B());
```

```cpp
std::vector<A> v;
v.push_back(A{});
v.push_back(B{});   // ?
```

A: The first is valid because all elements are treated as `Base*` and resolved at runtime via virtual dispatch. The second is invalid because `std::vector<A>` can only store objects of type `A`, and `B` is a different type.

---

---

### Optional Alternative (Compile-Time Union)

You can approximate this with templates using `std::variant`:

```cpp
std::vector<std::variant<A, B>> objects;
```

But:
- All possible types must be known at compile time
- This is not true runtime polymorphism

---

### Key Insight

> Templates are flexible at compile time  
> Virtual functions are flexible at runtime

### Quick Check

**Q: Why can virtual functions handle heterogeneous collections but templates cannot?**  
A: Because virtual functions use runtime polymorphism, allowing different types through a common base class.

---

## 6. Type Safety

### Templates

- Strong type checking at compile time
- Errors appear early

---

### Virtual Functions

- More flexible but less strict
- Errors may appear at runtime if misused

---

### Key Insight

> Templates catch errors earlier

### Quick Check

**Q: Which approach provides better compile-time safety?**  
A: Templates.

---

## 7. Code Size

### Templates

- Can generate multiple versions
- May increase binary size

---

### Virtual Functions

- Single implementation reused
- Smaller binary

---

### Key Insight

> Templates trade code size for performance

### Quick Check

**Q: Why can templates increase binary size?**  
A: Because each type generates a separate instantiation.

---

## 8. When to Use Each

### Use Templates When:

- Performance is critical
- Types are known at compile time
- Writing generic libraries
- No need for runtime polymorphism

---

### Use Virtual Functions When:

- Type is unknown at runtime
- Need polymorphic containers
- Plugin systems / extensibility
- Runtime behavior switching

---

### Quick Check

**Q: When should you prefer virtual functions over templates?**  
A: When behavior must be determined at runtime.

---

## 9. Side-by-Side Comparison

| Feature | Templates | Virtual Functions |
|--------|----------|------------------|
| Polymorphism | Compile-time | Runtime |
| Performance | High (no overhead) | Slight overhead |
| Flexibility | Compile-time only | Runtime |
| Code size | Larger | Smaller |
| Inlining | Yes | No (usually) |
| Use case | Generic programming | OOP polymorphism |

---

## 10. Mental Model

> Templates = "Generate the best code for each type"  
> Virtual = "Decide which behavior to use at runtime"

---

## 11. Real-World Insight

Templates power:
- STL (`std::vector`, `std::array`)
- Numeric libraries
- High-performance systems

Virtual functions power:
- GUI frameworks
- Plugin systems
- Game engines (object hierarchies)

---

## 12. Final Mental Anchor

- Templates = compile-time polymorphism  
- Virtual = runtime polymorphism  
- Templates = performance  
- Virtual = flexibility  

---

### Final Quick Check

**Q: Can templates replace virtual functions completely?**  
A: No. Templates cannot handle runtime polymorphism where types are not known at compile time.