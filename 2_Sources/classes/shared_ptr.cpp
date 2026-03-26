#include <iostream>

template < typename T>
class SharedPtr
{
    private:
        T* ptr_;
        size_t* ref_count_; // Must be a pointer to share count per resource

        void cleanup()
        {
            if (ref_count_)
            {
                (*ref_count_)--;
                if (*ref_count_ == 0)
                {
                    delete ptr_; 
                    delete ref_count_;
                }
            }
        }

    public:
        // Default constructor
        SharedPtr() noexcept : ptr_(nullptr), ref_count_(nullptr) {}

        // Constructor from raw pointer
        explicit SharedPtr(T* ptr) : ptr_(ptr)
        {
            ref_count_ = new int(1);
        }

        // Destructor
        ~SharedPtr() noexcept
        {
            cleanup();
        }

        // Copy constructor
        SharedPtr(const SharedPtr& other)
            : ptr_(other.ptr_), ref_count_(other.ref_count_)
        {
            if (ref_count_)
            {
                (*ref_count_)++;
            }
        }

        // Copy assignment
        SharedPtr& operator=(const SharedPtr& other)
        {
            if (this != &other)
            {
                cleanup(); // Release current resource

                ptr_ = other.ptr_;
                ref_count_ = other.ref_count_;
                if (ref_count_)
                {
                    (*ref_count_)++;
                }
            }
            return *this;
        }

        // Move constructor
        SharedPtr(SharedPtr&& other) noexcept
            : ptr_(other.ptr_), ref_count_(other.ref_count_)
        {
            other.ptr_ = nullptr;
            other.ref_count_ = nullptr;
        }

        // Move assignment
        SharedPtr& operator=(SharedPtr&& other) noexcept
        {
            if (this != &other)
            {
                cleanup();

                ptr_ = other.ptr_;
                ref_count_ = other.ref_count_;
                other.ptr_ = nullptr;
                other.ref_count_ = nullptr;
            }
            return *this;
        }

        T& operator*() const { return *ptr_; }
        T* operator->() const { return ptr_; }
        size_t use_count() const { return ref_count_ ? *ref_count_ : 0; }
};