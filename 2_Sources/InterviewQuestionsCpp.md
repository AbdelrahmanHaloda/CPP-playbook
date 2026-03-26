# Scenario-Based C++ Interview Questions with Answers

## 1) Returning a large vector by value in a hot path

**Question**
You profile a performance-critical module and find this function in a tight loop:

Latency spikes occur intermittently. What do you inspect first?

```cpp
std::vector<int> buildData() {
    std::vector<int> data;
    // fill with thousands of elements
    return data;
}
```

**Answer**
Returning by value is not automatically the problem in modern C++. The compiler may apply copy elision / NRVO (Named Return Value Optimization), and if not, `std::vector` is usually moved, not deeply copied.

The first thing to inspect is:

- repeated heap allocation while filling the vector
- missing reserve()
- rebuilding the vector every call
- downstream copies after return
- variable input size causing unpredictable allocation behavior

A good first fix is:

```cpp
std::vector<int> data;
data.reserve(expected_size);
```

If the path is latency-sensitive, consider:

- reusing a caller-owned buffer
- reusing the same vector across calls
- fixed-capacity storage if requirements allow

---

## 2) What is NRVO? Is it the same as copy elision?

**Question**
Explain NRVO and how it relates to copy elision.

**Answer**
NRVO stands for Named Return Value Optimization. It is a specific form of copy elision.

Example:

```cpp
std::vector<int> buildData() {
    std::vector<int> data;
    return data;
}
```

Here data is a named local variable. With NRVO, the compiler can construct the returned object directly in the caller’s storage, skipping copy/move.

So:

copy elision = broad category
NRVO = specific case when returning a named local variable

---

## 3) Why can reserve() reduce latency spikes?

**Question**
Why can std::vector::reserve() reduce intermittent latency spikes?

**Answer**
Without `reserve()`, repeated `push_back()` may trigger reallocation when capacity is exceeded. Reallocation typically does:

- allocate new memory
- move/copy existing elements
- free old memory

That makes most pushes cheap, but some pushes suddenly expensive, which creates timing spikes.

`reserve(n)` allocates enough capacity upfront, reducing or eliminating growth reallocations and making execution more predictable.

Important distinction:

- reserve(n) changes capacity
- resize(n) changes size

---

## 4) Even with NRVO and reserve, what cost remains?

**Question**
Given:

```cpp
std::vector<int> buildData() {
    std::vector<int> data;
    data.reserve(1000);
    for (int i = 0; i < 1000; ++i)
        data.push_back(i);
    return data;
}
```

Assume perfect NRVO. What cost still remains?

**Answer**
Even if the return itself is free, the function still costs:

- one allocation from reserve(1000) if capacity was initially zero
- 1000 loop iterations
- 1000 push_back() calls
- 1000 integer writes into memory
- memory/cache traffic

**reserve()** removes repeated growth reallocations, but it does not remove the cost of building the vector.

---

## 5) Returning a reference to a local object

**Question**
What is wrong with this function?

```cpp
const std::string& getName() {
    std::string name = "sensor_A";
    return name;
}
```

**Answer**
This returns a reference to a local object. The local std::string is destroyed when the function returns, so the caller receives a dangling reference. Using it is undefined behavior.

It may sometimes appear to work because the memory may not be overwritten immediately, but the program is still invalid.

Correct redesign:

```cpp
std::string getName() {
    return "sensor_A";
}
```

Returning by value is safe and efficient in modern C++.

## 6) Storing a reference member initialized from a temporary

**Question**
Is this safe?

```cpp
class Logger {
public:
    Logger(const std::string& prefix) : prefix_(prefix) {}
    const std::string& prefix() const { return prefix_; }

private:
    const std::string& prefix_;
};

Logger log(std::string("NET"));
```

**Answer**
No. This is broken.

`std::string("NET")` is a temporary. The constructor parameter binds to it, and the class stores a reference to it. The temporary lives only until the end of the full expression:

```cpp
Logger log(std::string("NET"));
```

After that line, prefix_ is a dangling reference.

Correct design:

```cpp
class Logger {
public:
    Logger(std::string prefix) : prefix_(std::move(prefix)) {}
    const std::string& prefix() const { return prefix_; }

private:
    std::string prefix_;
};
```

Store by value unless lifetime is externally guaranteed.

---

## 7) Returning a reference into a temporary container

**Question**
Is this safe?

**Answer**
No. getNames() returns a temporary vector. operator[] returns a reference to an element inside that temporary vector. When the full expression ends, the vector is destroyed, so the returned reference dangles.

Safer version:

```cpp
std::string getFirst() {
    return getNames()[0];
}
```

---

## 8) Moving from an object and then using it

**Question**
What is the state of s here?

```cpp
std::vector<std::string> v;
std::string s = "hello";

v.push_back(s);
v.push_back(std::move(s));
```

**Answer**
After std::move(s), s is in a valid but unspecified state.

That means:

- it can be destroyed
- assigned to
- cleared
- safely used in ways valid for any string object

But you must not assume its value. It is not guaranteed to be empty.

v will typically contain:

v[0] == "hello" from the copy
v[1] == "hello" from the move

Danger appears when code uses `s` afterward as if it still contains the original value.

## 9) Overload selection with lvalue/rvalue references

**Question**
Which overload is selected?

```cpp
void process(const std::string& name);
void process(std::string&& name);

std::string getName();

std::string s = "device";
process(s);
process(std::move(s));
process(getName());
```

**Answer**

- `process(s);` → `process(const std::string&)` because `s` is an lvalue
- `process(std::move(s));` → `process(std::string&&)` because `std::move(s)` is an xvalue/rvalue
- `process(getName());` → `process(std::string&&)` because a returned-by-value string is an rvalue

Important: std::move does not move by itself. It only casts to an rvalue category.

## 10) std::move does not remove const

### Question

What happens here?

```cpp
void handle(std::string& s);
void handle(const std::string& s);
void handle(std::string&& s);

const std::string b = "y";
handle(std::move(b));
```

### Answer

`std::move(b)` becomes `const std::string&&`, not `std::string&&`.

std::move does not remove const.

So:

cannot bind to std::string&&
can bind to const std::string&

Therefore it calls:

handle(const std::string&)

This is a very common trap.

## 11) Copy elision vs move constructor

### Question

What outputs are possible here?

```cpp
struct Buffer {
    Buffer() = default;
    Buffer(const Buffer&) { std::cout << "copy\n"; }
    Buffer(Buffer&&) noexcept { std::cout << "move\n"; }
};

Buffer make() {
    Buffer b;
    return b;
}

int main() {
    Buffer x = make();
}
```

### Answer

Possible outputs:

- no output
- move
- less commonly copy

Why:

- With copy elision / NRVO, the compiler constructs directly into `x`, so no copy or move runs
- If elision does not happen, the local object is usually moved
- Copy is the fallback if move is unavailable or not selected

Important lesson:
Returning by value does not mean a move must occur.

## 12) By-value parameter and copy/move construction

### Question

Explain what happens here:

```cpp
struct X {
    X() { std::cout << "default\n"; }
    X(const X&) { std::cout << "copy\n"; }
    X(X&&) noexcept { std::cout << "move\n"; }
};

void take(X x) {}

X a;
take(a);
take(std::move(a));
take(X{});
```

### Answer

- `X a;` → default
- `take(a);` → copy, because `a` is an lvalue
- `take(std::move(a));` → move, because the argument is an rvalue
- `take(X{});` → default, and the move into parameter `x` may be elided

For take(X{}), a temporary is created with the default constructor, and in practice it may be constructed directly as the parameter.

## 13) Forwarding reference wrapper bug

### Question

What is wrong here?

```cpp
template<typename T>
void wrapper(T&& arg) {
    process(arg);
}
```

### Answer

T&& here is a forwarding reference because T is deduced.

But inside the function, arg is a named variable, and named variables are lvalues as expressions. So:

process(arg);

always passes an lvalue, even if the caller passed an rvalue.

Correct version:

```cpp
template<typename T>
void wrapper(T&& arg) {
    process(std::forward<T>(arg));
}
```

std::forward<T>(arg) preserves the caller’s original value category.

## 14) Forwarding + const rvalue trap

### Question

Given:

```cpp
void process(const std::string&);
void process(std::string&&);

template<typename T>
void wrapper(T&& arg) {
    process(std::forward<T>(arg));
}

std::string s = "abc";
const std::string cs = "xyz";

Which overload is selected?

wrapper(s);
wrapper(cs);
wrapper(std::move(s));
wrapper(std::move(cs));
wrapper(std::string{"tmp"});
```

### Answer

- `wrapper(s);` → `process(const std::string&)`
- `wrapper(cs);` → `process(const std::string&)`
- `wrapper(std::move(s));` → `process(std::string&&)`
- `wrapper(std::move(cs));` → `process(const std::string&)`
- `wrapper(std::string{"tmp"});` → `process(std::string&&)`

Why std::move(cs) goes to const lvalue ref:

- `cs` is const
- `std::move(cs)` becomes `const std::string&&`
- cannot bind to `std::string&&`
- can bind to `const std::string&`

## 15) Pass-by-value constructor + move

### Question

Explain what happens here:

```cpp
struct Data {
    std::string name;
    Data(std::string n) : name(std::move(n)) {}
};

std::string s = "sensor";
Data d1(s);
Data d2(std::move(s));
```

### Answer

For d1(s):

- `s` is an lvalue
- parameter `n` is copy-constructed from `s`
- member `name` is move-constructed from `n`
- original `s` remains unchanged

For d2(std::move(s)):

- parameter `n` is move-constructed from `s`
- member `name` is move-constructed from `n`
- original `s` becomes valid but unspecified

This pattern is good when the class needs ownership because one constructor handles both lvalues and rvalues cleanly.

## 16) Two overloads vs pass-by-value constructor

### Question

Compare:

```cpp
Widget(const std::string& n) : name(n) {}
Widget(std::string&& n) : name(std::move(n)) {}
```

with:

```cpp
Widget(std::string n) : name(std::move(n)) {}
```

### Answer

Two-overload design:

- can be slightly more optimal
- lvalue copies directly into member
- rvalue moves directly into member

But:

- more code
- more maintenance
- more chance for inconsistency between overloads

Pass-by-value + move:

- simpler API
- one implementation
- lvalue pays one copy + one move
- rvalue usually pays moves only

In most practical code, pass-by-value + move is often a very good tradeoff.

## 17) Virtual dispatch inside a non-virtual function

### Question

What happens here?

```cpp
class Base {
public:
    virtual void process() { std::cout << "Base\n"; }
    void run() { process(); }
};

class Derived : public Base {
public:
    void process() override { std::cout << "Derived\n"; }
};

Base* ptr = new Derived();
ptr->run();
```

### Answer

Output:
`Derived`

Why:

- `run()` itself is non-virtual, so `Base::run()` is called
- inside `run()`, `process()` is virtual
- virtual dispatch uses the dynamic type of the object, which is `Derived`

Important lesson:
A virtual function remains virtual even when called from a non-virtual member function.

## 18) Deleting through a base pointer without virtual destructor

### Question

What is the bug here?

```cpp
class Base {
public:
    ~Base() { std::cout << "Base dtor\n"; }
};

class Derived : public Base {
public:
    ~Derived() { std::cout << "Derived dtor\n"; }
};

Base* ptr = new Derived();
delete ptr;
```

### Answer

This is undefined behavior because Base has a non-virtual destructor and the object is deleted through a base pointer.

Typical output:
`Base dtor`

The derived destructor may not run, so resources owned by Derived may leak.

Correct design:
```cpp
virtual ~Base() = default;
```

## 19) Non-virtual helper called from virtual function

### Question

What prints?

```cpp
class Base {
public:
    virtual void init() { setup(); }
    void setup() { std::cout << "Base setup\n"; }
};

class Derived : public Base {
public:
    void setup() { std::cout << "Derived setup\n"; }
};

Base* obj = new Derived();
obj->init();
```

### Answer

It prints:
`Base setup`

Why:

- `init()` is virtual, but `Derived` does not override it
- so `Base::init()` runs
- inside `Base::init()`, `setup()` is non-virtual
- non-virtual call is statically bound to `Base::setup()`

Common wrong assumption:
Calling a virtual function does not make all internal function calls polymorphic.

## 20) Accidental non-override due to different signature

### Question

Does this override?

```cpp
class Base {
public:
    virtual void foo() { std::cout << "Base foo\n"; }
};

class Derived : public Base {
public:
    void foo(int x) { std::cout << "Derived foo " << x << '\n'; }
};
```

### Answer

No. Derived::foo(int) does not override Base::foo() because the signatures differ.

So:

`Base* p = new Derived();`
`p->foo();`

prints:

`Base foo`

This demonstrates accidental non-override / name hiding.

Always use override when intending to override:

`void foo(int x) override; // compiler error`

---

## 21) Object slicing through by-value base parameter

### Question

What happens here?

```c++
class Engine {
public:
    virtual void start() { std::cout << "Engine start\n"; }
};

class ElectricEngine : public Engine {
public:
    void start() override { std::cout << "Electric start\n"; }
};

void boot(Engine engine) {
    engine.start();
}

ElectricEngine e;
boot(e);
```

### Answer

It prints:

Engine start

Because boot takes Engine by value. Passing ElectricEngine by value causes object slicing: only the base part is copied into the parameter. Inside boot, engine is a pure Engine object, so virtual dispatch uses dynamic type Engine.

Fix:

void boot(Engine& engine)

or

```cpp
void boot(Engine* engine);
```

---

## 22) Object slicing during assignment to base object

**Question**
What prints?

```cpp
class Base {
public:
    virtual void f() { std::cout << "Base\n"; }
};

class Derived : public Base {
public:
    void f() override { std::cout << "Derived\n"; }
};

Base b = Derived();
b.f();
```

**Answer**
It prints: `Base`

This is object slicing. `Derived()` creates a temporary derived object, but only the base subobject is used to initialize `b`. After slicing, `b` is a pure `Base` object.

---

## 23) Override failure due to const mismatch

**Question**
Does this override?

```cpp
class Base {
public:
    virtual void f() { std::cout << "Base\n"; }
};

class Derived : public Base {
public:
    void f() const { std::cout << "Derived\n"; }
};
```

**Answer**
No. The `const` makes it a different signature, so it does not override the base method.

Therefore:

```cpp
Base* p = new Derived();
p->f();
```

prints: `Base`

This bug is dangerous because the code looks correct but silently fails to override.

**Prevention:**
Always use the `override` keyword. The compiler would reject:

```cpp
void f() const override;
```

---

## 24) Ref-qualified member functions and static type trap

**Question**
Explain this behavior:

```cpp
class Base {
public:
    virtual void f() { std::cout << "Base\n"; }
};

class Derived : public Base {
public:
    void f() & override { std::cout << "Derived lvalue\n"; }
    void f() && override { std::cout << "Derived rvalue\n"; }
};
```

**Answer**

- `void f() &` can be called only on lvalues.
- `void f() &&` can be called only on rvalues.

This is an advanced topic. The main lesson is:
- Overload resolution happens first, based on the static type.
- Virtual dispatch happens second, based on the dynamic type.

If the base interface does not provide a callable function for the expression category, virtual dispatch never happens.

---

## 25) Pure polymorphism requires reference/pointer, not value

**Question**
Why does polymorphism break in code like this?

```cpp
void boot(Base b);
```

**Answer**
Because passing by value creates a new base object and slices off the derived part. Polymorphism works when the dynamic type is preserved, which requires:

- A base reference (`Base&`)
- A base pointer (`Base*`)

Passing by value destroys the polymorphic property of the object.

---

## 26) std::move on const objects in real systems

**Question**
Why can moving a `const` object be misleading?

**Answer**
Because `std::move(const T)` yields `const T&&`, not `T&&`. Most move constructors and move overloads require a non-const rvalue, so moving a `const` object often falls back to copy-like behavior or binds to `const T&`.

This matters in performance-sensitive code because developers may think they optimized a path, but the object is still copied.

---

## 27) Virtual destructor in interface-like base classes

**Question**
When should a base class destructor be virtual?

**Answer**
If the class is intended to be used polymorphically and objects may be deleted through a base pointer, the destructor **must** be virtual.

If not, deleting through base pointer is undefined behavior.

**Common safe pattern:**

```cpp
class Interface {
public:
    virtual ~Interface() = default;
    virtual void run() = 0;
};
```

---

## 28) Pass-by-reference vs pass-by-value in latency-sensitive systems

**Question**
When would you prefer caller-provided output buffers over returning by value?

**Answer**
In hard real-time or low-latency systems, caller-provided buffers can help:

- Avoid repeated heap allocations
- Reuse existing memory
- Make ownership explicit
- Reduce timing jitter

**Example:**

```cpp
void fillFrame(Frame& out);
```

This is less elegant than returning by value, but often more deterministic.

---

## 29) Why undefined behavior may appear to work

**Question**
Why do lifetime bugs sometimes appear fine in testing?

**Answer**
Because undefined behavior (UB) does not guarantee immediate failure. It may appear to work because:

- Stale memory has not been overwritten yet.
- Debug build memory layouts can hide the issue.
- Specific timing conditions aren't met.
- The bug only triggers under heavy load or specific compiler optimizations.

That is why lifetime bugs are dangerous in field systems.

---

## 30) override keyword importance

**Question**
Why should `override` be used whenever an override is intended?

**Answer**
Because it turns many silent runtime bugs into compile-time errors. It catches:

- Wrong parameter lists
- Wrong constness
- Wrong ref-qualifiers
- Typos in function names
- Accidental non-overrides (hiding)

In large C++ systems, this prevents subtle and hard-to-track polymorphism bugs.

---

# Short Embedded/Telecom-Specific Scenario Questions

---

## 31) Allocation in packet-processing path

**Question**
A packet-processing thread sometimes misses deadlines. The hot path uses `std::vector` growth and dynamic strings. What do you suspect?

**Answer**
I would suspect:

- Heap allocation jitter.
- Unpredicted `vector` reallocation.
- Frequent temporary object creation.
- Avoidable copies/moves.
- Allocator contention if the system is multithreaded.

**In a real-time path, I would consider:**

- Using `reserve()`.
- Object reuse/pooling.
- Preallocated buffers.
- Using fixed-capacity containers.

---

## 32) Passing polymorphic driver object by value

**Question**
A driver abstraction uses a base class, but behavior becomes generic after passing it into a helper function. Why?

**Answer**
Most likely object slicing. If the helper takes the base object by value, the derived part is lost and polymorphism breaks, meaning the base class's implementation is used instead of the driver's.

**Fix:** Pass by reference (`Base&`) or pointer (`Base*`).

---

## 33) Non-virtual destructor in hardware interface

**Question**
Why is a non-virtual destructor dangerous in a driver interface hierarchy?

**Answer**
Because derived classes may own critical resources such as:

- File descriptors
- Mapped memory
- Device handles
- Mutexes
- DMA buffers

Deleting via base pointer without virtual destructor can skip derived cleanup and cause leaks or inconsistent hardware state.

---

## 34) Callback captures dead object

**Question**
An async callback fires after the owning object is destroyed. What class of bug is this?

**Answer**
This is a **lifetime bug** (specifically a use-after-free). The callback holds a pointer or reference to an object that no longer exists.

**Typical fixes:**

- Ownership redesign.
- Cancellation of callbacks on destruction.
- Using the `std::weak_ptr` pattern.
- Defining a clear callback lifetime contract.

---

## 35) “Works in debug, fails in release”

**Question**
What C++ bug classes do you suspect first if code works in debug but fails in release?

**Answer**
The first suspects would be:

- Undefined behavior.
- Use-after-free.
- Dangling references/pointers.
- Uninitialized memory.
- Data races (timing changes).
- Relying on a moved-from value.
- Lifetime bugs previously hidden by debug memory layout.

---
