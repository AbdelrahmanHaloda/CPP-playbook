

// I thought I understood this.

// You're reviewing a co-workers code. They recently heard about smart pointers and have a use-case for std::shared_ptr in particular.

// You take a look at their code. Is anything wrong with it?

// Headers

class Executor;
struct Data { };

class Processor {
public:
    void ProcessStuff();
    Data ProduceData() { /* Generate data here. */ return { }; }
    std::shared_ptr<Processor> GetSharedPtr() {
        return std::shared_ptr<Processor>(this);
    }
private:
    Executor* executor_;
};

class Executor {
public:
    void Execute(Processor* processor) {
        work_.push([self = processor->GetSharedPtr()] {
            // Do something with self,
            // such as call ProduceData and use it.
        });
    }
private:
    // A method passed to a std::thread object
    // that pops off the work_ and handles it.
    void ConsumeData() {
        // Something gets done here with Data.
    }
private:
    // Other data members.
    std::queue<std::function<void()>> work_;
};

void Processor::ProcessStuff() {
    executor_->Execute(this);
}

class Client {
public:
    // Called on occasion.
    void Something() {
        processor_->ProcessStuff();
    }

private:
    std::shared_ptr<Processor> processor_{ new Processor() };
};


/*Yes, something is wrong. Processor should not create a shared_ptr from this.

To start, we must capture Processor as a shared_ptr in the lambda.

Imagine the case where the processing thread in Executor is backed up, and processor_ in Client is reset after being captured in the lambda . If we passed in Processor as this pointer instead of a strong-reference counting shared_ptr, this pointer would be a dangling pointer by the time the lambda is executed.

Now that we have that out of the way, why can't Processor make a shared_ptr to itself, and how can we fix it?

GetSharedPtr() is unsafe because std::shared_ptr<Processor>(this) creates a new owning shared_ptr from a raw pointer, which can lead to double-delete. Ouch, we don't want that.

We can solve this with a rarely discussed C++ construct called std::enable_shared_from_this, which is a CRTP template built to allow for the creation of a shared_ptr object to this through a control block that is managed through a weak_ptr.

If we write Processor as such:

C++
class Processor : public std::enable_shared_from_this<Processor> {
}; 

it now gains a shared_from_this() method.

Inside, std::enabled_shared_from_this, a weak_ptr to itself is kept, and a shared_ptr can be acquired from that when needed. It looks something like this:

C++
template <typename T>
class enable_shared_from_this {
    mutable weak_ptr<T> weak_this;
public:
    shared_ptr<T> shared_from_this() {
        return shared_ptr<T>(weak_this); 
     }

    //const overload
    shared_ptr<const T> shared_from_this() const {
        return shared_ptr<const T>(weak_this); 
     }

   //..more methods and constructors..
   //there is weak_from_this() also since C++17

    template <class U> friend class shared_ptr;
};

The rest of the magic lies in the constructor of shared_ptr, which detects whether T inherits from enable_shared_from_this via type-traits (like std::is_convertible) and initializes weak_this.
*/