# Perfect Forwarding in C++

## Core Pattern

Perfect forwarding means passing an argument to another function while preserving how the caller originally passed it:

* Its value category: lvalue or rvalue
* Its `const` qualification

```cpp
template<typename T>
void wrapper(T&& value)
{
    target(std::forward<T>(value));
}
```

The two parts have different jobs:

```text
T&&
→ receives the argument and records its original category in T

std::forward<T>(value)
→ restores that category when passing value onward
```

## Types and Expression Categories

Do not mix **types** with **expression categories**.

### Types

Examples:

```cpp
std::string
const std::string

std::string&
const std::string&

std::string&&
const std::string&&
```

`&` and `&&` are parts of a type.

### Expression Categories

The possible value categories are:

```text
lvalue
xvalue
prvalue
```

An **rvalue** is either an xvalue or a prvalue.

```text
rvalue = xvalue or prvalue
```

Examples:

```cpp
std::string name = "Ahmed";

name;                 // lvalue
std::move(name);      // xvalue
std::string{"Ahmed"}; // prvalue
```

A named variable is an lvalue expression, regardless of its declared type:

```cpp
std::string&& value = std::string{"Ahmed"};
```

```text
Declared type of value = std::string&&
Category of expression value = lvalue
```

Therefore:

```cpp
target(value);            // passes an lvalue
target(std::move(value)); // passes an xvalue
```

Keep this distinction permanent:

```text
std::string&&            → type
lvalue, xvalue, prvalue  → expression categories
```

## Template Deduction Depends on the Parameter Pattern

The argument alone does not determine `T`.

The compiler considers:

1. The argument's underlying type
2. The argument's value category
3. The function parameter pattern

Given:

```cpp
std::string name = "Ahmed";
```

The expression `name` is an lvalue of underlying type `std::string`.

| Parameter pattern | Deduced `T`    |
| ----------------- | -------------- |
| `T`               | `std::string`  |
| `T&`              | `std::string`  |
| `const T&`        | `std::string`  |
| Forwarding `T&&`  | `std::string&` |

The important result is:

> The same argument can produce different deductions because template deduction depends on the parameter pattern.

## Equation Mental Model

Ask:

> What must `T` contribute so the complete parameter can bind to the caller's argument?

Think of it as:

```text
T contribution + parameter reference = required reference
```

### Pattern `T&` Receiving an Lvalue

An lvalue requires `&`:

```text
T + & = &
```

The pattern already supplies `&`, so `T` remains the underlying type:

```text
T = A
```

### Pattern `T&&` Receiving an Rvalue

An rvalue can bind to `&&`:

```text
T + && = &&
```

The pattern already supplies `&&`, so:

```text
T = A
```

### Forwarding `T&&` Receiving an Lvalue

An lvalue requires `&`, but the pattern supplies `&&`:

```text
T + && = &
```

Therefore, `T` must contribute `&`:

```text
T = A&
```

Then:

```text
A& && → A&
```

### Mental Process

```text
1. Determine the reference required by the argument.
2. Check what the parameter pattern already provides.
3. If the pattern already satisfies the requirement, keep T as A.
4. If forwarding T&& receives an lvalue, deduce T as A&.
5. Substitute T into the parameter pattern.
6. Apply reference collapsing.
```

## Forwarding-Reference Deduction

For this exact pattern:

```cpp
template<typename T>
void wrapper(T&& value);
```

when `T` is deduced from that function argument:

```text
lvalue A       → T = A&
const lvalue A → T = const A&
rvalue A       → T = A
const rvalue A → T = const A
```

Examples:

```cpp
std::string name = "Ahmed";
const std::string constName = "Ahmed";

wrapper(name);                 // T = std::string&
wrapper(constName);            // T = const std::string&
wrapper(std::move(name));      // T = std::string
wrapper(std::move(constName)); // T = const std::string
```

## Reference Collapsing

```text
&  + &  → &
&  + && → &
&& + &  → &
&& + && → &&
```

Memorable rule:

> If either reference is `&`, the result is `&`. Only `&&` with `&&` remains `&&`.

## Fixed and Forwarding References

A fixed rvalue reference accepts only rvalues:

```cpp
void function(std::string&& value);
```

```cpp
std::string name = "Ahmed";

function(name);            // error
function(std::move(name)); // valid
```

A forwarding reference can accept lvalues and rvalues:

```cpp
template<typename T>
void function(T&& value);
```

```cpp
function(name);            // valid
function(std::move(name)); // valid
```

The pattern is a forwarding reference only when `T` is deduced from that exact function argument.

```cpp
template<typename T>
void function(const T&& value);
```

`const T&&` is not a forwarding reference. It is an rvalue-reference pattern.

## How `std::forward` Works

Conceptually:

```cpp
std::forward<T>(value)
```

behaves like:

```cpp
static_cast<T&&>(value)
```

### When `T = A&`

```cpp
static_cast<T&&>(value)
```

becomes:

```cpp
static_cast<A& &&>(value)
```

After collapsing:

```cpp
static_cast<A&>(value)
```

Result:

```text
decltype(std::forward<T>(value)) = A&
Expression category = lvalue
```

### When `T = A`

```cpp
static_cast<T&&>(value)
```

becomes:

```cpp
static_cast<A&&>(value)
```

Result:

```text
decltype(std::forward<T>(value)) = A&&
Expression category = xvalue
```

`std::forward` does not inspect the original caller directly. It uses the information stored in `T`.

## Analysis Pattern

For every forwarding question, follow this order:

```text
1. Argument underlying type
2. Argument value category
3. Parameter pattern
4. Deduced T
5. Declared parameter type after collapsing
6. Category of the named parameter expression
7. Expansion of std::forward as static_cast<T&&>
8. Category of the forwarded expression
9. Selected overload
```

## Challenge 1: Lvalue Argument

```cpp
#include <string>
#include <utility>

void target(const std::string&);
void target(std::string&&);

template<typename T>
void wrapper(T&& value)
{
    target(std::forward<T>(value));
}

int main()
{
    std::string name = "Ahmed";
    wrapper(name);
}
```

### Challenge 1 Analysis

| Step                               | Result                       |
| ---------------------------------- | ---------------------------- |
| Argument underlying type           | `std::string`                |
| Argument category                  | lvalue                       |
| Deduced `T`                        | `std::string&`               |
| Declared type of `value`           | `std::string&`               |
| Category of `value`                | lvalue                       |
| `decltype(std::forward<T>(value))` | `std::string&`               |
| Forwarded category                 | lvalue                       |
| Selected overload                  | `target(const std::string&)` |

Reasoning:

```text
T = std::string&

T&&
→ std::string& &&
→ std::string&
```

Forwarding:

```text
std::forward<T>(value)
→ static_cast<std::string& &&>(value)
→ static_cast<std::string&>(value)
→ lvalue
```

## Challenge 2: Non-Const Xvalue

```cpp
#include <string>
#include <utility>

void target(const std::string&);
void target(std::string&&);

template<typename T>
void wrapper(T&& value)
{
    target(std::forward<T>(value));
}

int main()
{
    std::string name = "Ahmed";
    wrapper(std::move(name));
}
```

### Challenge 2 Analysis

| Step                               | Result                  |
| ---------------------------------- | ----------------------- |
| Argument underlying type           | `std::string`           |
| Argument category                  | xvalue                  |
| Deduced `T`                        | `std::string`           |
| Declared type of `value`           | `std::string&&`         |
| Category of `value`                | lvalue                  |
| `decltype(std::forward<T>(value))` | `std::string&&`         |
| Forwarded category                 | xvalue                  |
| Selected overload                  | `target(std::string&&)` |

Reasoning:

```text
T = std::string

T&&
→ std::string&&
```

Although `value` has type `std::string&&`, the expression `value` is an lvalue because it is named.

Forwarding restores the original category:

```text
std::forward<T>(value)
→ static_cast<std::string&&>(value)
→ xvalue
```

## Challenge 3: Const Xvalue

```cpp
#include <string>
#include <utility>

void target(std::string&);
void target(const std::string&);
void target(std::string&&);
void target(const std::string&&);

template<typename T>
void wrapper(T&& value)
{
    target(std::forward<T>(value));
}

int main()
{
    const std::string name = "Ahmed";
    wrapper(std::move(name));
}
```

### Challenge 3 Analysis

| Step                               | Result                        |
| ---------------------------------- | ----------------------------- |
| Argument underlying type           | `const std::string`           |
| Argument category                  | xvalue                        |
| Deduced `T`                        | `const std::string`           |
| Declared type of `value`           | `const std::string&&`         |
| Category of `value`                | lvalue                        |
| `decltype(std::forward<T>(value))` | `const std::string&&`         |
| Forwarded category                 | xvalue                        |
| Selected overload                  | `target(const std::string&&)` |

Reasoning:

```text
std::move(name)
→ changes the category to xvalue
→ does not remove const
```

Deduction:

```text
T = const std::string
```

Parameter formation:

```text
T&&
→ const std::string&&
```

The named parameter remains an lvalue expression:

```text
value
→ lvalue
```

Forwarding:

```text
std::forward<T>(value)
→ static_cast<const std::string&&>(value)
→ const xvalue
```

Therefore, the exact matching overload is:

```cpp
void target(const std::string&&);
```

## Final Summary

```text
Template deduction depends on the parameter pattern.

For forwarding T&&:

lvalue A       → T = A&
const lvalue A → T = const A&
rvalue A       → T = A
const rvalue A → T = const A

After deduction:
1. Substitute T into T&&.
2. Apply reference collapsing.
3. This determines the parameter's declared type.

Inside the function:
A named parameter is always an lvalue expression.

std::forward<T>(value):
1. Approximately casts value to T&&.
2. Restores the category stored inside T.
3. Preserves const.
```

Never confuse:

```text
&, &&                   → parts of types
lvalue, xvalue, prvalue → expression categories
```
