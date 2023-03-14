#include <iostream>
template <typename Buffer>
class BufferIterator {
public:
    using value_type = typename Buffer::value_type;
    using pointer = value_type*;
    using reference = value_type&;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::random_access_iterator_tag;

private:
    pointer _data;
    pointer _mem_begin;
    pointer _data_begin;
    size_t _capacity = 0;

public:
    BufferIterator() = default;
    BufferIterator(pointer my_data, pointer my_memory, pointer my_data_begin, size_t cap) : 
        _data(my_data), _mem_begin(my_memory), _data_begin(my_data_begin), _capacity(cap) {}
    
    BufferIterator& operator++() {        
        _data ++;
        if (_data == _mem_begin + _capacity)
            _data = _mem_begin;
        return *this; 
    }

    BufferIterator operator++(int) {
        BufferIterator ret = *this;
        ++(*this);
        return ret;
    }

    BufferIterator& operator--() {
        if(_data == _mem_begin)
            _data = _mem_begin + _capacity;
        _data --;
        return *this;
    }

    BufferIterator operator--(int) {
        BufferIterator ret = *this;
        --(*this);
        return ret;
    }
    
     difference_type operator-(const BufferIterator& other) const { 
        return _data - other._data; 
    }

    BufferIterator operator-(const difference_type other) const {
        if (_data - other < _mem_begin)
            return BufferIterator(_data + (_capacity - other), _mem_begin, _data_begin, _capacity);
        return BufferIterator(_data - other, _mem_begin, _data_begin, _capacity);
    }

    BufferIterator operator+(const difference_type other) const {
        difference_type  off = (_data + other) - (_mem_begin + _capacity);
        if (_data + other >= _mem_begin + _capacity) 
            return BufferIterator(_data + (other - _capacity), _mem_begin, _data_begin, _capacity);
        return BufferIterator(_data + other, _mem_begin, _data_begin, _capacity);
    }

    // BufferIterator operator+(const BufferIterator& other) const {
    //     return BufferIterator(_data + other);
    // }   
    
    BufferIterator& operator+=(const difference_type other) {
        if (_data + other >= _mem_begin + _capacity)
            _data -= _capacity;
        _data += other;
        return *this;
    }

    BufferIterator& operator-=(const difference_type other) {
        if (_data - other < _mem_begin)
            _data += _capacity;
        _data -= other;
        return *this;
    }

    bool operator==(const BufferIterator& other) const {
        return _data == other._data;
    }

    bool operator!=(BufferIterator other) const {
        return !(*this == other);
    }
    
    bool operator<(const BufferIterator& other) const {
        if (_data_begin <= _data && _data_begin <= other._data)
            return _data < other._data;
        if (_data_begin >= _data && _data_begin >= other._data)
            return _data < other._data;
        return _data > other._data;
    }

    bool operator<=(const BufferIterator& other) const {
        return (*this < other) || (*this == other);
    }
    
    bool operator>(const BufferIterator& other) const {
        if (_data_begin <= _data && _data_begin <= other._data)
            return _data > other.data;
        if (_data_begin >= _data && _data_begin >= other._data)
            return _data > other._data;
        return _data < other._data;
    }
    
    bool operator>=(const BufferIterator& other) const {
        return (*this > other) || (*this == other);
    }

    reference operator*() {
        return *_data;
    }

    pointer operator->() {
        return _data;
    }

    reference operator[](const difference_type ind) {
        return *(*this + ind);
    }
};

template <typename T>
class Buffer {
public:
    using value_type = T;
    using Iterator = BufferIterator<Buffer<T>>;

private:
    T* _mem_begin = nullptr;
    T* _data_begin = nullptr;
    size_t _size = 0;
    size_t _capacity = 0;

public:
    Buffer() {
        ReAlloc(2);
    }

    ~Buffer() {
        Clear();
        //::operator delete(_mem_begin, _capacity * sizeof(T));
        //::operator delete(_mem_begin);
        //delete _mem_begin;
    }

    void PushBack(const T& val) {
        if(_size == _capacity - 1)
            ReAlloc(2 * _capacity);

        new(&(*end())) T(val); //placement new
        _size ++;
    }

    void PushBack(T&& val) {
        if(_size == _capacity - 1)
            ReAlloc(2 * _capacity);
        
        new(&(*end())) T(std::move(val)); //placement new
        _size ++;
    }

    template<typename... Args>
    T& EmplaceBack(Args&&... args) {
        if(_size == _capacity - 1)
            ReAlloc(2 * _capacity);
        
        new(&(*end())) T(std::forward<Args>(args)...); //placement new
        return *(begin() + (_size++));
    }

    void PushFront(const T& val) {
         if(_size == _capacity - 1)
            ReAlloc(2 * _capacity);

        _data_begin = &(*(begin() - 1));
        *_data_begin = val;
        _size++;
    }

    void PopFront() {
        _data_begin[0].~T();
        _data_begin = &(*(begin() + 1));
        _size --;
    }

    void PopBack() {
        *(end()-1).~T();
        _size--;
    }

    void Clear() {
        for (auto it = begin() ; it != end() ; it++)
            (*it).~T();
        _size = 0;
    }

    size_t Size() const {
        return _size;
    }
    
    size_t Capacity() const {
        return _capacity;
    }

    const T& operator[](size_t index) const {
        //provjera
        return *(begin() + index);
    }

    T& operator[](size_t index) {
        //provjera
        return *(begin() + index);
    }

    Iterator begin() {
        return Iterator(_data_begin, _mem_begin, _data_begin, _capacity);
    }

    Iterator end() {
        return begin() + _size;
    }

private:
    void ReAlloc(size_t new_capacity) {
        std::cout << "Reallocating " << new_capacity << std::endl;

        T* new_mem = (T*)::operator new(new_capacity * sizeof(T));
        for (size_t i = 0; i < _size; i ++)
            new(&new_mem[i]) T(std::move(_data_begin[i]));
        
        for (size_t i = 0; i < _size; i ++)
            _data_begin[i].~T();

        ::operator delete(_mem_begin, _capacity * sizeof(T));

        _mem_begin = new_mem;
        _data_begin = new_mem;
        _capacity = new_capacity;
    }
};
