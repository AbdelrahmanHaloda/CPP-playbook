

class Abdelrahman
{
    private:
        /* data */
        int* _pData;
        int _size;
    
    public:

        // constructor
        Abdelrahman(int size) : _size(size)
        {
            _pData = new int[size];
            // Constructor code here
        }   

        // destructor
        ~Abdelrahman()
        {
            if (_pData != nullptr)
            {
                delete[] _pData;
            }
            // Destructor code here
        }

        // copy constructor
        Abdelrahman(const Abdelrahman& source)
        {
            _size = source._size;
            
            // perform deep copy of the data
            _pData = new int[_size];
            std::copy(source._pData, source._pData + _size, _pData);
            // Copy constructor code here
        }

        // copy assignment operator
        Abdelrahman& operator=(const Abdelrahman& source)
        {
            // gurard self check
            if (this == &source)
            {
                return *this;
            }
            else
            {
                // perform deep copy of the data
                _size = source._size;
                delete[] _pData;
                _pData = new int[_size];
                std::copy(source._pData, source._pData + _size, _pData);
                // Copy assignment operator code here
            }
            return *this;
        }

        // move constructor
        Abdelrahman(Abdelrahman&& source) noexcept : _size(source._size), _pData(source._pData)
        {
            // release the source's resources
            source._size = 0;
            source._pData = nullptr;
            // Move constructor code here
        }

        // move assignment operator
        Abdelrahman& operator=(Abdelrahman&& source) noexcept
        {
            // gurard self check
            if (this == &source)
            {
                return *this;
            }
            else
            {
                // release own resources
                delete[] _pData;
                
                // steal/copy
                _size = source._size;
                _pData = source._pData;

                // release the source's resources
                source._size = 0;
                source._pData = nullptr;
                // Move assignment operator code here
            }
            return *this;
        }
};