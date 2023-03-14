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
    int _data;
    pointer _mem_begin;
    size_t _front;
    size_t _capacity;

public:
    BufferIterator() = default;
    BufferIterator(int my_data, pointer my_memory, size_t _buffer_front, size_t cap) : 
        _data(my_data), _mem_begin(my_memory), _front(_buffer_front), _capacity(cap) {}
    
    BufferIterator& operator++() {        
        _data = (_data + 1) % _capacity;
        return *this; 
    }

    BufferIterator operator++(int) {
        BufferIterator ret = *this;
        ++(*this);
        return ret;
    }

    BufferIterator& operator--() {
        _data = (_data - 1 + _capacity) % _capacity;
        return *this;
    }

    BufferIterator operator--(int) {
        BufferIterator ret = *this;
        --(*this);
        return ret;
    }
    
    difference_type operator-(const BufferIterator& other) const {
        if (_front <= _data && _front <= other._data)
            return _data - other._data;
        if (_front > _data && _front > other._data)
            return _data - other._data;
        
        if (_data < other._data) 
            return _data + _capacity - other._data;
        return -(_data + _capacity - other._data);
    }

    BufferIterator operator-(const difference_type other) const {
        return BufferIterator((_data - other + _capacity) % _capacity, _mem_begin, _front, _capacity);
    }

    BufferIterator operator+(const difference_type other) const {
        return BufferIterator((_data + other) % _capacity, _mem_begin, _front, _capacity);
    }
    
    BufferIterator& operator+=(const difference_type other) {
        _data = (_data + other) % _capacity;
        return *this;
    }

    BufferIterator& operator-=(const difference_type other) {
        _data = (_data - other) % _capacity;
        return *this;
    }

    bool operator==(const BufferIterator& other) const {
        return _data == other._data;
    }

    bool operator!=(BufferIterator other) const {
        return !(*this == other);
    }
    
    bool operator<(const BufferIterator& other) const {
        if (_front <= _data && _front <= other._data)
            return _data < other._data;
        if (_front > _data && _front > other._data)
            return _data < other._data;
        return _data > other._data;
    }

    bool operator<=(const BufferIterator& other) const {
        return (*this < other) || (*this == other);
    }
    
    bool operator>(const BufferIterator& other) const {
        if (_front <= _data && _front <= other._data)
            return _data > other.data;
        if (_front > _data && _front > other._data)
            return _data > other._data;
        return _data < other._data;
    }
    
    bool operator>=(const BufferIterator& other) const {
        return (*this > other) || (*this == other);
    }

    reference operator*() {
        return _mem_begin[_data];
    }

    pointer operator->() {
        return &_mem_begin[_data];
    }

    reference operator[](const difference_type ind) {
        return _mem_begin[(_data+ind)%_capacity];
    }
};

template <typename T>
class Buffer {
public:
    using value_type = T;
    using Iterator = BufferIterator<Buffer<T>>;

private:
    T* _mem_begin = nullptr;
    size_t _front = 0;
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
    }

    void PushBack(const T& val) {
        if(_size == _capacity - 1) // jer begin() == end() ako _size = _capacity
            ReAlloc(2 * _capacity);

        new(&_mem_begin[(_front + _size)%_capacity]) T(val); //placement new
        _size ++;
    }

    void PushBack(T&& val) {
        if(_size == _capacity - 1)
            ReAlloc(2 * _capacity);
        
        new(&_mem_begin[(_front + _size)%_capacity]) T(std::move(val)); //placement new
        _size ++;
    }

    template<typename... Args>
    T& EmplaceBack(Args&&... args) {
        if(_size == _capacity - 1)
            ReAlloc(2 * _capacity);
        
        new(&_mem_begin[(_front + _size)%_capacity]) T(std::forward<Args>(args)...); //placement new
        return _mem_begin[(_front + _size ++)%_capacity];
    }

    void PushFront(const T& val) {
         if(_size == _capacity - 1)
            ReAlloc(2 * _capacity);

        _front = (_front - 1 + _capacity) % _capacity;
        _mem_begin[_front] = val;
        _size ++;
    }

    void PopFront() {
        _mem_begin[_front].~T();
        _front = (_front + 1) % _capacity;
        _size --;
    }

    void PopBack() {
        _mem_begin[(_front + _size - 1) % _capacity].~T();
        _size--;
    }

    void Clear() {
       for (size_t i = 0; i < _size; i ++)
            _mem_begin[(_front + i)%_capacity].~T();
        _size = 0;
    }

    size_t Size() const {
        return _size;
    }
    
    size_t Capacity() const {
        return _capacity;
    }

    bool Empty() const {
        return _size == 0;
    }

    const T& operator[](size_t index) const {
        return _mem_begin[(_front + index)%_capacity];
    }

    T& operator[](size_t index) {
        return _mem_begin[(_front + index)%_capacity];
    }

    Iterator begin() {
        return Iterator(_front, _mem_begin, _front, _capacity);
    }

    Iterator end() {
        return begin() + _size;
    }

private:
    void ReAlloc(size_t new_capacity) {
        std::cout << "Reallocating " << new_capacity << std::endl;

        T* new_mem = (T*)::operator new(new_capacity * sizeof(T));
        for (size_t i = 0; i < _size; i ++)
            new(&new_mem[i]) T(std::move(
                _mem_begin[(_front + i)%_capacity]
            ));
        
        for (size_t i = 0; i < _size; i ++)
            _mem_begin[(_front + i)%_capacity].~T();

        ::operator delete(_mem_begin, _capacity * sizeof(T));

        _mem_begin = new_mem;
        _front = 0;
        _capacity = new_capacity;
    }
};