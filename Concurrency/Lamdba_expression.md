### <u>Lambda expression "Lambda" for short</u>

With a Lambda you can easily create simple function objects.

The name **"Lambda"** comes from **Lambda Calculus** , a mathematical formalism invented by **Alonzo Church** in the 1930s to investigate questions of logic and computability. **Lambda calculus** formed the basis of **LISP**, a functional programming language. Compared to Lambda Calculus and LISP, **C ++ - Lambdas have the properties of being unnamed and capturing variables from the surrounding context, but lack the ability to execute and return functions.**

A **Lambda** is often used as an argument for functions that can take a callable object. This can be easier than creating a named function that is used only when passed as an argument. In such cases, **Lambdas** are generally preferred because they allow the function objects to be defined inline. If **Lambdas** were not available, we would have to define an extra function somewhere else in our source file - which would work but at the expense of the clarity of the source code.

**A Lambda is a function object (a "functor")**, so it has a type and can be stored and passed around. Its result object is called a **"closure"**, which can be called using the operator ().

**A lambda formally consists of three parts:** 


**- A main part {}** 
which contains the code to be executed when the Lambda is called. 


**- A parameter list ()** 
The way parameters are passed to a Lambda is basically identical to calling a regular function. If the Lambda takes no arguments, these parentheses can be omitted (except when "mutable" is used).

**- A capture list []** 
By default, variables outside of the enclosing {} around the main part of the Lambda can not be accessed. 
By adding a variable to the capture list, it becomes available within the Lambda either as a copy or as a reference. The captured variables become a part of the Lambda.

By default, variables in the capture block can not be modified within the Lambda. Using the keyword **"mutable"** allows to modify the parameters captured by copy, and to call their non-const member functions within the body of the Lambda. 

**Note that in principal all parts could be empty.**

**Example 1:**
The following code examples show several ways of making the external variable "id" accessible within a Lambda.

```
#include <iostream>

int main()
{
    // create lambdas
    int id = 0; // Define an integer variable

    //auto f0 = []() { std::cout << "ID = " << id << std::endl; }; // Error: 'id' cannot be accessed

    id++;
    auto f1 = [id]() { std::cout << "ID = " << id << std::endl; }; // OK, 'id' is captured by value

    id++;
    auto f2 = [&id]() { std::cout << "ID = " << id << std::endl; }; // OK, 'id' is captured by reference

    //auto f3 = [id]() { std::cout << "ID = " << ++id << std::endl; }; // Error, 'id' may not be modified

    auto f4 = [id]() mutable { std::cout << "ID = " << ++id << std::endl; }; // OK, 'id' may be modified

    // execute lambdas
    f1();
    f2();
    f4();

    return 0;
}
```
**Example 1 o/p:**

```
ID = 1
ID = 2
ID = 3
```
Even though we have been using Lambdas in the above example in various ways, it is important to note that a Lambda does not exist at runtime. The runtime effect of a Lambda is the generation of an object, which is known as **closure**. **The difference between a Lambda and the corresponding closure is similar to the distinction between a class and an instance of the class**. 
**A class exists only in the source code while the objects created from it exist at runtime.**

We can use (a copy of) the closure (i.e. f0, f1, …) to execute the code within the Lambda at a position in our program different to the line where the function object was created.

**Example 2:**
The following example illustrates how the function object is first created and then used to pass the parameter id later in the code.

```
#include <iostream>

int main()
{
    int id = 0; // Define an integer variable

    // create lambda
    auto f = [](const int id) { std::cout << "ID = " << id << std::endl; }; // ID is passed as a parameter

    // execute function object and pass the parameter
    f(id);

    return 0;
}
```

**Example 2 o/p:**
```
ID = 0
```

**Example 3:**

```
#include <iostream>

int main()
{
    int id = 0; // Define an integer variable

    // capture by reference (immutable)
    auto f0 = [&id]() { std::cout << "a) ID in Lambda = " << id << std::endl; };

    // capture by value (mutable)
    auto f1 = [id]() mutable { std::cout << "b) ID in Lambda = " << ++id << std::endl; };
    f1(); // call the closure and execute the code witin the Lambda
    std::cout << "c) ID in Main = " << id << std::endl;

    // capture by reference (mutable)
    auto f2 = [&id]() mutable { std::cout << "d) ID in Lambda = " << ++id << std::endl; };
    f2(); 
    std::cout << "e) ID in Main = " << id << std::endl; 

    // pass parameter 
    auto f3 = [](const int id) { std::cout << "f) ID in Lambda = " << id << std::endl; };   
    f3(++id);

    // observe the effect of capturing by reference at an earlier point in time
    f0(); 

    return 0;
}
```

**Example 3 o/p:**

```
b) ID in Lambda = 1
c) ID in Main = 0
d) ID in Lambda = 1
e) ID in Main = 1
f) ID in Lambda = 2
a) ID in Lambda = 2
```

---