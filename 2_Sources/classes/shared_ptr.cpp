#include <iostream>

template <typename T>
class SharedPtr
{
    private:
        T* ptr;
        size_t* refCount_; // Must be a pointer to share count per resource

        // Clean
        void clean()
        {
            if(refCount)
            {
                (*refCount)--;
                if(*refCount == 0)
                {
                    delete ptr;
                    delete refCount;
                }
            }
            ptr = nullptr;
            refCount = nullptr;
        }

    public:

        // Default Constructor
        SharedPtr() noexcept : 
            ptr(nullptr),
            refCount(nullptr)
            {}

        // Constructor with raw pointer
        explicit SharedPtr(T* p) :
            ptr(p),
            refCount(p ? new std::size_t(1) : nullptr)
            {}


        // Destructor
        ~SharedPtr()
        {
            clean();
        }

        // copy constructor
        SharedPtr(const SharedPtr& other) noexcept :
            ptr(other.ptr),
            refCount(other.refCount)
        {
            if(refCount)
            {
                (*refCount)++;
            }
        }

        // copy assignment operator
        SharedPtr& operator=(const SharedPtr& other) noexcept
        {
            if (this != &other)
            {
                clean();
                
                ptr = other.ptr;
                refCount = other.refCount;
                
                if (refCount)
                {
                    (*refCount)++;
                }
            }
            return *this;
        }

        // move constructor
        SharedPtr(SharedPtr&& other) noexcept :
            ptr(other.ptr),
            refCount(other.refCount)
        {
            other.ptr = nullptr;
            other.refCount = nullptr;
        }

        // move assignment operator
        SharedPtr& operator=(SharedPtr&& other) noexcept
        {
            if (this != &other)
            {
                clean();

                ptr = other.ptr;
                refCount = other.refCount;
                
                other.ptr = nullptr;
                other.refCount = nullptr;
            }
            return *this;
        }

        // dereference operator
        T& operator*() const noexcept
        {
            return *ptr;
        }

        // arrow operator
        T* operator->() const noexcept
        {
            return ptr;
        }

        // get
        T* get() const noexcept
        {
            return ptr;
        }

};

int main ()
{

    return 1;
}