/*
 * Thread-Safe Blocking Ring Buffer
 *
 * Design:
 * - The buffer has a fixed capacity and stores elements in FIFO order.
 * - The write index `in` points to the next insertion position.
 * - The read index `out` points to the next element to remove.
 * - Both indices wrap back to zero when they reach the buffer capacity.
 * - `count` tracks the current number of stored elements.
 *
 * Thread safety:
 * - A single `std::mutex` protects all shared mutable state:
 *   `buffer`, `in`, `out`, and `count`.
 * - `push()` waits when the buffer is full.
 * - `pop()` waits when the buffer is empty.
 *
 * Condition variables:
 * - `not_empty` wakes a waiting consumer after an element is pushed.
 * - `not_full` wakes a waiting producer after an element is popped.
 * - Predicates are used during waiting to handle early notifications
 *   and spurious wakeups safely.
 *
 * Locking:
 * - `std::unique_lock` is used in `push()` and `pop()` because
 *   condition variables must temporarily unlock the mutex while waiting.
 * - The mutex is explicitly unlocked before notifying another thread,
 *   allowing the awakened thread to acquire it immediately.
 * - `std::lock_guard` is used in operations that only need normal
 *   scope-based locking, such as `size()` and `printBuffer()`.
 *
 * Current API behavior:
 * - `push()` is blocking: it waits until space becomes available.
 * - `pop()` is blocking: it waits until an element becomes available.
 * - Therefore, tests should use separate producer and consumer threads.
 */

 #include <iostream>
using namespace std;


template<typename T>

class RingBuffer
{
    public:
        // constructor
        RingBuffer(int capacity) : 
        in(0),
        out(0),
        count(0),
        size_(capacity)
        {
            buffer.resize(size_);
        }

        bool push(T data)
        {
            bool retCode = false;

            // lock_guard<mutex> lock(m_mutex);
            unique_lock<mutex> lock(m_mutex);

            not_full.wait(lock, [this](){
                return !isFull();
            });

            // another check for more safety
            if (!isFull())
            {
                if (in == size_)
                {
                    in = 0;
                }
                buffer[in] = data;
                in++;
                count++;
                retCode = true;
            }

            lock.unlock();
            not_empty.notify_one();

            return retCode;
        }

        bool pop(T& data)
        {
            bool retCode = false;
            
            // lock_guard<mutex> lock(m_mutex);
            unique_lock<mutex> lock(m_mutex);

            not_empty.wait(lock, [this](){

                return !isEmpty();
            });

            // double check after wakepup
            if(!isEmpty())
            {
                if (out == size_)
                {
                    out = 0;
                }
                data = std::move(buffer[out]);
                out++;
                count--;
                retCode = true;
            }
            lock.unlock();
            not_full.notify_one();

            return retCode;
        }

        int size()
        {
            lock_guard<mutex> lock(m_mutex);
            return count;
        }

        int capacity()
        {
            return size_;
        }

        bool printBuffer()
        {
            bool retCode = false;

            lock_guard<mutex> lock(m_mutex);
            
            if (!isEmpty())
            {
                int temp = out % size_;
                for(size_t i= 0; i < count; ++i)
                {
                    cout<<buffer[temp]<<endl;
                    temp = (temp +1) % size_;
                }
                retCode = true;
            }
            return retCode;
        }

    private:
        int in;
        int out;
        int size_;
        int count;
        vector<T> buffer{};
        mutex m_mutex;
        condition_variable not_empty;
        condition_variable not_full;
        

        bool isEmpty()
        {
            return count == 0;
        }

        bool isFull()
        {
            return count == size_;
        }

};


void buffeffill(int size, RingBuffer<int>& buffer)
{
    // add 3,4,5
    for(size_t i = 0; i<size; ++i)
    {
        if(buffer.push(i+1))
        {
            cout<<"true"<<" value = "<<i+1<<endl;
        }
        else
        {
            cout<<"false"<<endl;
        }
    }
}



int main ()
{
    int size = 3;
    RingBuffer <int>buffer(size);


    // fill out buffer with 1 2 3
    buffeffill(size, buffer);
    if (buffer.push(5))
    {
        cout<<"true"<<endl;
    }
    else
    {
        cout<<"false"<<endl;
    }
    buffer.printBuffer();
    int x = 0;
    buffer.pop(x);
    buffer.printBuffer();

    


    return 1;
}