/*
You are implementing a command-line application for an automotive parking system.

The application receives the sensor type as the first command-line argument.

Example:

./parking camera
./parking radar
./parking ultrasound

Each sensor has its own processing algorithm.

Initially, the system supports:

Camera
Radar
Ultrasound

Requirements

Redesign the system to satisfy the following requirements:

- Avoid if / else if / switch statements for selecting the algorithm. 
check

- Follow the Open/Closed Principle: Adding a new sensor (for example, LiDAR) should not require modifying the dispatching logic.
check

- Use runtime polymorphism so all sensors share a common interface.
check

- Implement a registry that maps a sensor name (string) to an object creator.

- If an unknown sensor name is provided, the application should still return a valid object instead of nullptr (Use the Null Object Pattern to encapsulate the behavior for unsupported sensors)

Use modern C++:
- std::unique_ptr
- std::unordered_map
- Lambdas
- std::function

Demonstrate the design in main() by reading the sensor name from argv[1].
*/

#include<iostream>
#include <memory>
#include<functional>
#include <unordered_map>
#include <string>

// implement c++



// base class
class Sensor
{
    public:
        virtual void Execute() = 0;
        virtual ~Sensor() = default;
};


class US : public Sensor
{
public:
    virtual void Execute() override
    {
        std::cout << "Executing Ultrasound Logic\n";
    }
};

class Radar : public Sensor
{
public:
    virtual void Execute() override
    {
        std::cout << "Executing Radar Logic\n";
    }
};

class Camera : public Sensor
{
public:
    virtual void Execute() override
    {
        std::cout << "Executing Camera Logic\n";
    }
};

class UnsupportedObject : public Sensor
{
public:
    virtual void Execute() override
    {
        std::cout << "Unsupported Camera sensor\n";
    }
};

class SensorRegistry
{
public:

    void Register(std::string name, std::function<std::unique_ptr<Sensor>()> lambda)
    {
        dic[name] = lambda;
    }

    std::unique_ptr<Sensor> Instantiate(const std::string& name)
    {
        auto i = dic.find(name);

        if (i != dic.end())
        {
            return i->second();
        }

        // when sensor name is invalid or not supported return null object.
        return std::make_unique<UnsupportedObject>();
        
    }

private:
    std::unordered_map<std::string, std::function<std::unique_ptr<Sensor>()>> dic{};

};



class ParkingSystem
{
public:

    ParkingSystem(std::unique_ptr<Sensor> sensor_) : sensor(std::move(sensor_))
    {

    }

    void Execute()
    {
        sensor->Execute();
    }

private:
    std::unique_ptr<Sensor> sensor;
};


 
int main(int argc, char* argv[])
{
    // create Sensor registery object
    SensorRegistry registry;

    // parse the input string and store it in a string variable
    std::string inputStr{};

    // store all possible sensors in the registery --> string name, function that creates the correct object 
    registry.Register("camera", [](){
        return std::make_unique<Camera>();
    });

    registry.Register("radar", [](){
        return std::make_unique<Radar>();
    });

    registry.Register("us", [](){
        return std::make_unique<US>();
    });


    // get the camera sesnor based on input string
    std::unique_ptr<Sensor> camSensor = registry.Instantiate(inputStr);

    ParkingSystem PS(std::move(camSensor));

    PS.Execute();
 
}