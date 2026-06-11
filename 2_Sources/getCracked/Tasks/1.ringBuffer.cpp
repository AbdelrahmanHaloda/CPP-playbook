// Implement a Ring Buffer
// MediumLanguage KnowledgeCTC
// Description

// Implement a generic circular buffer (also called a ring buffer) with a fixed capacity. A circular buffer stores elements in a fixed-size array, reusing slots as items are consumed. When the buffer is full, writing a new item overwrites the oldest item, ensuring the most recent data is always available.

// Do not concern yourself with thread-safety.
// Constraints

//     If the buffer is empty and read is called, return nullptr. Likewise, try_read returns false.

//     reset wipes out all elements in the buffer.

//     You must be able to disambiguate between the full and empty cases.


// Write your solution here
// C++23 using GCC 14.2
// Debug with std::cerr or std::clog.
// !!! IMPORTANT !!!
// 99% of headers are pre-compiled for you server-side.
// If your submission fails to compile due to a missing header, add it to your submission.

template <typename T>
class CircularBuffer {
public:
    CircularBuffer() 
    :   in_(0),
        out_(0),
        count_(0) {};

    explicit CircularBuffer(std::size_t capacity)
    :   in_(0),
        out_(0),
        count_(0),
        capacity_(capacity) 
    {
        buffer.resize(capacity);
    }

    std::shared_ptr<T> read()
    {
        T result;

        if(try_read(result))
        {
            return std::make_shared<T>(result);
        }
        else
        {
            return nullptr;
        }
    }
    
    // write an item to the buffer, if full don't overwrite the oldest item
    void write(const T& item)
    {
        if (!full())
        {
            buffer[in_] = item;
            in_++;
            count_++;
            if (in_ == capacity_) 
            {
                in_ = 0;
            }
        }
    }

    // write an item to the buffer, overwriting the oldest item if the buffer is full    
/*    void write(const T& item)
    {
        buffer[in_] = item;

        if (full()) 
        {
            out_++;
            if (out_ == capacity_) 
            {
                out_ = 0;
            }
        } 
        else 
        {
            count_++;
        }

        in_++;
        if (in_ == capacity_) {
            in_ = 0;
        }
    }*/   
    
    bool try_read(T& item)
    {
        bool retCode = false;  
        if(!empty())
        {
            retCode = true;
            item = buffer[out_];
            out_ ++;
            count_ --;
            if(out_ == capacity_)
            {
                out_ = 0;
            }
        }

        return retCode;
    }
    
    void reset()
    {
        in_ = 0;
        out_ = 0;
        count_ = 0;
    }

    // check if full
    [[nodiscard]] bool full()  const
    {
        return count_ == capacity_;
    }

    // check if empty
    [[nodiscard]] bool empty() const
    {
        return count_ == 0;
    }

    // get current buffer size
    [[nodiscard]] std::size_t size() const
    {
        return count_;
    }

    // get buffer max capacity
    [[nodiscard]] std::size_t capacity() const
    {
        return capacity_;
    }

private:
    std::vector<T> buffer;
    std::size_t in_;
    std::size_t out_;
    std::size_t count_;
    std::size_t capacity_;
};








