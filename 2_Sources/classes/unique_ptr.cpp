#include <algorithm>
#include <iostream>
#include <utility>
#include <cassert>

template <typename T>
class UniquePtr
{
    private:
        T* ptr_;

    public:
        // default constructor
        UniquePtr() noexcept
            : ptr_(nullptr) { }

        // constructor from raw pointer
        explicit UniquePtr(T* ptr) noexcept
            : ptr_(ptr) { }

        // destructor
        ~UniquePtr() noexcept
        {
            delete ptr_;
        }

        // delete copy constructor
        UniquePtr(const UniquePtr&) = delete;
        // delete copy assignment
        UniquePtr& operator=(const UniquePtr&) = delete;

        // move constructor
        UniquePtr(UniquePtr&& other) noexcept
            : ptr_(other.ptr_)
        {
            other.ptr_ = nullptr;
        }

        // move assignment
        UniquePtr& operator=(UniquePtr&& other) noexcept
        {
            if (this != &other)
            {
                delete ptr_;           // free current resource
                ptr_ = other.ptr_;     // steal ownership
                other.ptr_ = nullptr;  // leave source empty
            }
            return *this;
        }

        // dereference
        T& operator*() const noexcept
        {
            return *ptr_;
        }

        // member access
        T* operator->() const noexcept
        {
            return ptr_;
        }

        // get raw pointer
        T* get() const noexcept
        {
            return ptr_;
        }

        // release ownership without deleting
        T* release() noexcept
        {
            T* temp = ptr_;
            ptr_ = nullptr;
            return temp;
        }

        // replace managed object
        void reset(T* newPtr = nullptr) noexcept
        {
            if (ptr_ != newPtr)
            {
                delete ptr_;
                ptr_ = newPtr;
            }
        }

        // swap
        void swap(UniquePtr& other) noexcept
        {
            std::swap(ptr_, other.ptr_);
        }

        // bool conversion
        explicit operator bool() const noexcept
        {
            return ptr_ != nullptr;
        }
};

class Test
{
public:
    Test(int value)
        : value_(value)
    {
        std::cout << "Test constructed: " << value_ << '\n';
    }

    ~Test()
    {
        std::cout << "Test destroyed: " << value_ << '\n';
    }

    void print() const
    {
        std::cout << "value = " << value_ << '\n';
    }

private:
    int value_;
};

int main()
{
    UniquePtr<Test> p1(new Test(10));

    if (p1)
    {
        p1->print();
    }

    UniquePtr<Test> p2 = std::move(p1);

    if (!p1)
    {
        std::cout << "p1 is empty after move\n";
    }

    if (p2)
    {
        p2->print();
    }

    p2.reset(new Test(20));
    p2->print();

    Test* raw = p2.release();
    if (!p2)
    {
        std::cout << "p2 is empty after release\n";
    }

    raw->print();
    delete raw;

    return 0;
}

/*
What makes this a unique_ptr:

only one owner at a time
copying is forbidden
moving transfers ownership
destructor cleans up automatically

What interviewers usually want you to explain:

Why copy is deleted
because two smart pointers owning the same raw pointer would cause double delete
Why move is allowed
ownership can be transferred safely from one object to another
Why moved-from object is set to nullptr
to make sure old owner no longer deletes the resource
Why destructor uses delete
because this version manages a single object allocated with new

Common follow-up traps:

this version does not support arrays like new T[]
this version does not support custom deleters
operator* on null is invalid, so assert is used here
self move-assignment should not break anything

A stronger interview version is to also add make_unique:
*/
