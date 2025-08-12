#include <iostream>

// base class
class Vehicle
{
    public:
        // Best practice: Polymorphic base classes should have a virtual destructor.
        virtual ~Vehicle() = default;

        virtual void move()
        {
            std::cout << "Vehicle Moving\n";
        }
};

// ISSUE 1 SOLVED: Use 'virtual' inheritance.
// This ensures that Jetskey gets only ONE shared instance of the Vehicle base class,
// preventing duplicate data members from the common ancestor.
class Car : public virtual Vehicle
{
    public:
        void move() override
        {
            std::cout << "Car Moving\n";
        }

};


class Boat : public virtual Vehicle
{
    public:
        void move() override
        {
            std::cout << "Boat Moving\n";
        }
};


class Jetskey : public Boat, public Car
{
    public:
        // ISSUE 2 SOLVED: Override the ambiguous function in the final class.
        // This resolves the ambiguity of which 'move()' to call (Boat's or Car's)
        // by providing a single, definitive version for Jetskey.
        void move() override {
            std::cout << "Jetskey moving!\n";
        }
};

int main()
{
    // --- Scenario 1: The Correct Solution (Compiles and Runs) ---
    // This works because both issues of the diamond problem are solved:
    // 1. 'virtual' inheritance prevents multiple 'Vehicle' subobjects.
    // 2. 'Jetskey::move()' override resolves the function ambiguity.
    std::cout << "--- Scenario 1: Correct Solution ---\n";
    Jetskey jk;
    jk.move(); // Calls the final overrider in Jetskey

    // Polymorphic use is also safe now
    Vehicle* pVehicle = &jk; // This conversion is unambiguous
    pVehicle->move();        // Calls Jetskey::move() via the v-table
    std::cout << "\n";


    // =============================================================================
    // TO SEE THE ERRORS, UNCOMMENT ONE OF THE FOLLOWING SCENARIOS AT A TIME
    // AND MAKE THE CODE CHANGES DESCRIBED IN THE COMMENTS.
    // =============================================================================


    // --- Scenario 2: Failure due to Ambiguous Function Call (Issue #2) ---
    // To test: Comment out the 'move()' function inside the 'Jetskey' class.
    // ERROR: "request for member 'move' is ambiguous"
    // Why it fails: The compiler knows 'Jetskey' should have a 'move()' function,
    // but it inherits two different versions (from Car and Boat) and has no
    // final overrider to resolve the conflict.
    /*
    std::cout << "--- Scenario 2: Ambiguous Function Call ---\n";
    Jetskey jk_fail2;
    jk_fail2.move(); // This line causes the error
    */


    // --- Scenario 3: Failure due to Ambiguous Base Class (Issue #1) ---
    // To test: Remove the 'virtual' keyword from the inheritance of 'Car' and 'Boat'.
    // ERROR: "'Vehicle' is an ambiguous base of 'Jetskey'"
    // Why it fails: Without 'virtual' inheritance, a 'Jetskey' object contains two
    // separate 'Vehicle' subobjects. When trying to create a 'Vehicle*' pointer,
    // the compiler doesn't know which of the two subobjects to point to.
    /*
    std::cout << "--- Scenario 3: Ambiguous Base Class ---\n";
    Jetskey jk_fail3;
    Vehicle* pVehicle_fail3 = &jk_fail3; // This line causes the error
    */

    return 0;
}