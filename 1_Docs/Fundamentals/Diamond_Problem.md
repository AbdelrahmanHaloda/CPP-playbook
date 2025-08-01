## Diamond Problem
The **"diamond problem"** is a term used in object-oriented programming, particularly in languages like C++ that support multiple inheritance. It refers to an issue that arises when a class inherits from two or more classes that have a common base class

Consider the following example:

```
/* Vehicle abstract Class */
class Vehicle{
    public:
    virtual void Move() = 0;
};

/* Boat class  inherts from Vehicle class */
class Boat : public Vehicle{
    public:
    void Move()override{
        // logic A
    };
};


/* Car class  inherts from Vehicle class */
class Car : public Vehicle{
    public:
    void Move()override{
        // logic B
    };
};

/* AmphibiousCar class inherts from Boat and Car Class */
class AmphibiousCar : public Boat, public Car{
};

int main(){
    AmphibiousCar amphibious;
    amphibious.Move();  // Compilation Error: "error: request for member 'Move' is ambiguous"
}
```
**o/p**

```
/tmp/o9JxK2zOXk.cpp:33:16: error: request for member 'Move' is ambiguous
   33 |     amphibious.Move();  // Conflict between the Move() API (Boat or Car !!)
      |                ^~~~
/tmp/o9JxK2zOXk.cpp:7:18: note: candidates are: 'virtual void Vehicle::Move()'
    7 |     virtual void Move() = 0;
      |                  ^~~~
/tmp/o9JxK2zOXk.cpp:22:10: note:                 'virtual void Car::Move()'
   22 |     void Move()override{
      |          ^~~~
/tmp/o9JxK2zOXk.cpp:13:10: note:                 'virtual void Boat::Move()'
   13 |     void Move()override{
      |          ^~~~
```


