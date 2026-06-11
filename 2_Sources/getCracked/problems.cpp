

// (1) const char* mission

// Observe the code snippet below. What gets printed, if anything?

// Headers
int main()
{
    std::map<const char*, int> m{};
    m["a"] = 5;
    auto* p1 = "a";
    std::string s = "abcdefghij";
    auto* p2 = s.data();
    std::cout << m["a"];
    std::cout << m[p1];
    std::cout << m[p2]; 
}

/*
The core question here is whether all references to the string literal "a" are interned (point to the same underlying read-only instance of said character).

Whether or not this will happen is not specified in the language. Indeed, different compilers will exhibit different behavior.

For instance, gcc 16.1 ad clang 22.1 both intern p1 with "a" (with zero optimizations). msvc v19.35 on the otherhand does not intern p1 without optimizations. With 2 levels of optimization (\02), it interns them.

In other words, without optimization, gcc and clang print 550, while msvc prints 000.
*/

// ----


// (2) Minus 0

// Does C++ support negative zero?

/*
In C++, negative zero (-0.0) is a valid value for floating-point types (float, double, long double), but it typically does not exist for integer types.

Modern C++ compilers follow the IEEE 754 standard, which represents floating-point numbers using a sign bit, an exponent, and a mantissa. Negative zero is represented by setting the sign bit to 1 while the exponent and mantissa bits are all zero. You can check if this bit is active when testing zero using std::signbit.

Using the == operator, -0.0 is equal to +0.0.

The key behavioral differences are:

- Division: Dividing a positive number by +0.0yields positive infinity (inf), while dividing by -0.0 yields negative infinity (-inf).
- Arithmetic: Generally, adding 0.0 to a negative zero value will result in +0.0, but multiplying a positive zero by a negative number will result in -0.0.
*/

// --


