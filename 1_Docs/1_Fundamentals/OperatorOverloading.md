
## Operator Overloading
It can be done on any operator from the ASCII table and give it your own set of rules!

In order to overload an operator, use the operator keyword in the function signature:

```c++
ReturnType operatorOp(const ClassName& other) {
  /*
  * where Op is the operator to be overloaded
  * It can be +, - , *, /, or any operator
  */
  //...logic to overload the operator
}
```