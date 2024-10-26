## Open/Closed Principle (OCP)

**Example 1: Violation of OCP**
In this version, the DiscountCalculator class needs to be modified whenever we introduce a new discount type, violating the **Open/Closed Principle**. 
The class is not closed for modification.

```
#include <string>

class DiscountCalculator {
public:
    double calculateDiscount(const std::string& customerType, double amount) {
        if (customerType == "Regular") {
            return amount * 0.1;  // 10% discount
        } else if (customerType == "Premium") {
            return amount * 0.2;  // 20% discount
        }
        return 0.0;  // No discount
    }
};
```

**Problem:**
Each time a new customer type is introduced (e.g., "VIP"), we need to modify the DiscountCalculator class, which risks introducing bugs and makes the code harder to maintain.


**Example 2: Adheres to OCP**
In this version, the OCP is respected. We use polymorphism to extend the behavior without modifying existing code. Now, new discount types can be added by creating new classes.

```
class Discount {
public:
    virtual ~Discount() = default;
    virtual double apply(double amount) const = 0;
};

class RegularDiscount : public Discount {
public:
    double apply(double amount) const override {
        return amount * 0.1;  // 10% discount
    }
};

class PremiumDiscount : public Discount {
public:
    double apply(double amount) const override {
        return amount * 0.2;  // 20% discount
    }
};

class DiscountCalculator {
public:
    double calculateDiscount(const Discount& discount, double amount) const {
        return discount.apply(amount);
    }
};
```

**Usage:**
```
DiscountCalculator calculator;
RegularDiscount regular;
PremiumDiscount premium;

double regularAmount = calculator.calculateDiscount(regular, 100.0);
double premiumAmount = calculator.calculateDiscount(premium, 100.0);
```


**Explanation:**
Now, we can extend the system by adding new discount types (e.g., VIPDiscount) without modifying the existing code.
The DiscountCalculator class remains closed for modification but open for extension through new discount types.

---