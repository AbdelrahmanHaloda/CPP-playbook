## Liskov Substitution Principle (LSP)

**Example 1: Violation of LSP**
In this version, PercentageDiscount and FixedDiscount inherit from the same base class. However, a new NoDiscount subclass violates LSP because it behaves differently—returning a constant value without using the input amount.

```
#include <iostream>

class Discount {
public:
    virtual double apply(double amount) const = 0;
};

class PercentageDiscount : public Discount {
public:
    double apply(double amount) const override {
        return amount * 0.1;  // 10% discount
    }
};

class FixedDiscount : public Discount {
public:
    double apply(double amount) const override {
        return amount - 50;  // $50 discount
    }
};

// Violation: NoDiscount behaves inconsistently
class NoDiscount : public Discount {
public:
    double apply(double /*amount*/) const override {
        return 0.0;  // Always returns 0, ignoring the input amount
    }
};

int main() {
    PercentageDiscount percentage;
    FixedDiscount fixed;
    NoDiscount noDiscount;

    printDiscount(percentage, 100.0);  // Output: Discount: 10
    printDiscount(fixed, 100.0);       // Output: Discount: 50
    printDiscount(noDiscount, 100.0);  // Output: Discount: 0 (Unexpected behavior)
}
```

**Problem:**
- NoDiscount breaks **LSP** because it doesn’t follow the expected behavior.
- A consumer of Discount expects all subclasses to calculate discounts based on the input amount, but NoDiscount ignores it.

**Example 2: Adheres to LSP**
To adhere to LSP, we modify the design so that all discount types maintain consistent behavior. We use a Null Object pattern to handle the case where no discount is applied.

```
#include <iostream>

class Discount {
public:
    virtual ~Discount() = default;
    virtual double apply(double amount) const = 0;
};

class PercentageDiscount : public Discount {
public:
    double apply(double amount) const override {
        return amount * 0.1;  // 10% discount
    }
};

class FixedDiscount : public Discount {
public:
    double apply(double amount) const override {
        return amount - 50;  // $50 discount
    }
};

class NoDiscount : public Discount {
public:
    double apply(double amount) const override {
        return amount;  // No discount, return the original amount
    }
};

int main() {
    PercentageDiscount percentage;
    FixedDiscount fixed;
    NoDiscount noDiscount;

    printDiscount(percentage, 100.0);  // Output: Final Price: 90
    printDiscount(fixed, 100.0);       // Output: Final Price: 50
    printDiscount(noDiscount, 100.0);  // Output: Final Price: 100
}
```

**Explanation:**

- All subclasses now behave consistently by using the input amount in their calculations.
- The NoDiscount class correctly adheres to LSP by returning the original amount instead of a constant value.
- This design ensures that any Discount object can be used interchangeably without unexpected behavior, following the Liskov Substitution Principle.

---