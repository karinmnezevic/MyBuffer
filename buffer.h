#include <cassert>

/*
Buffer<T> is an stl-inspired container designed for efficient message processing.
It emphasizes conservative and well-managed usage of heap-allocated memory.

Its interface consist of the usual PushBack(), EmplaceBack(), PushFront(), PopBack() and PopFront() functions, all having O(1) complexity.
Furthermore, it allows for random access of elements via an stl-compatible BufferIterator, also with O(1) complexity.

The data is stored in a single dymanically-allocated contiguous block of memory.
Where:
    T* _mem_begin       ---> points to the beginning of the allocated chunk
    size_t _front       ---> "points"(relative to _mem_begin) to the current beginning of the buffer
    size_t _size        ---> keeps track of its length
    size_t _capacity    ---> signals the amount of allocated memory

As elements are added/removed to/from the buffer it grows/shrinks and shifts along the allocated memory,
overflowing from the memory's end to beginning or vice-versa (circular implementation).
If at some point we run out of space i.e. (_size = _capacity) we reallocate to twice the previous capacity
and move/copy the stored data. 

Also worth noting is that the construction of elements is done separately from allocating the required memory,
the same being the case with destruction/deallocation.
*/

template <typename Buffer>
class BufferIterator {
public:
    using value_type        = typename Buffer::value_type;
    using pointer           = value_type*;
    using reference         = value_type&;
    using difference_type   = std::ptrdiff_t;
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
        return BufferIterator(
            (_data - other + _capacity) % _capacity, _mem_begin, _front, _capacity
        );
    }

    BufferIterator operator+(const difference_type other) const {
        return BufferIterator(
            (_data + other) % _capacity, _mem_begin, _front, _capacity
        );
    }
    
    BufferIterator& operator+=(const difference_type other) {
        _data = (_data + other) % _capacity;
        return *this;
    }

    BufferIterator& operator-=(const difference_type other) {
        _data = (_data + _capacity - other) % _capacity;
        return *this;
    }

    bool operator==(const BufferIterator& other) const {
        return _data == other._data;
    }

    bool operator!=(const BufferIterator& other) const {
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
    using value_type        = T;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    using Iterator          = BufferIterator<Buffer<value_type>>;
    using cIterator         = BufferIterator<Buffer<const value_type>>;

private:
    pointer _mem_begin = nullptr;
    size_t _front = 0;
    size_t _size = 0;
    size_t _capacity = 0;

public:
    Buffer() {
        ReAlloc(2);
    }

    ~Buffer() {
        Clear();
        ::operator delete(_mem_begin, _capacity * sizeof(value_type));
    }

    void PushBack(const_reference val) {
        if(_size == _capacity - 1)
            ReAlloc(2 * _capacity);

        new(&_mem_begin[(_front + _size)%_capacity]) value_type(val); //placement new
        _size ++;
    }

    void PushBack(value_type&& val) {
        if(_size == _capacity - 1)
            ReAlloc(2 * _capacity);
        
        new(&_mem_begin[(_front + _size)%_capacity]) value_type(std::move(val)); //placement new
        _size ++;
    }

    template<typename... Args>
    reference EmplaceBack(Args&&... args) {
        if(_size == _capacity - 1)
            ReAlloc(2 * _capacity);
        
        new(&_mem_begin[(_front + _size)%_capacity]) 
            value_type(std::forward<Args>(args)...); //placement new
        return _mem_begin[(_front + _size ++)%_capacity];
    }

    void PushFront(const_reference val) {
         if(_size == _capacity - 1)
            ReAlloc(2 * _capacity);

        _front = (_front - 1 + _capacity) % _capacity;
        _mem_begin[_front] = val;
        _size ++;
    }

    void PopFront() {
        assert(_size > 0);
        _mem_begin[_front].~value_type();
        _front = (_front + 1) % _capacity;
        _size --;
    }

    void PopBack() {
        assert(_size > 0);
        _mem_begin[(_front + _size - 1) % _capacity].~value_type();
        _size--;
    }

    void Clear() {
       for (size_t i = 0; i < _size; i ++)
            _mem_begin[(_front + i)%_capacity].~value_type();
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

    const_reference operator[](size_t index) const {
        assert(index < _size);
        return _mem_begin[(_front + index)%_capacity];
    }

    reference operator[](size_t index) {
        assert(index < _size);
        return _mem_begin[(_front + index)%_capacity];
    }

    const_reference Front() const {
        assert(_size > 0);
        return _mem_begin[_front];
    }

    reference Front() {
        assert(_size > 0);
        return _mem_begin[_front];
    }

    const_reference Back() const {
        assert(_size > 0);
        return _mem_begin[(_front + _size - 1)%_capacity];
    }

    reference Back() {
        assert(_size > 0);
        return _mem_begin[(_front + _size - 1)%_capacity];
    }

    Iterator begin() {
        return Iterator(_front, _mem_begin, _front, _capacity);
    }

    Iterator end() {
        return begin() + _size;
    }

    cIterator cbegin() {
        return cIterator(_front, _mem_begin, _front, _capacity);
    }

    cIterator cend() {
        return cbegin() + _size;
    }

private:
    void ReAlloc(size_t new_capacity) {
        pointer new_mem = (pointer)::operator new(new_capacity * sizeof(value_type));
        for (size_t i = 0; i < _size; i ++)
            new(&new_mem[i]) value_type(std::move(
                _mem_begin[(_front + i)%_capacity]
            ));
        
        for (size_t i = 0; i < _size; i ++)
            _mem_begin[(_front + i)%_capacity].~value_type();
        ::operator delete(_mem_begin, _capacity * sizeof(value_type));

        _mem_begin = new_mem;
        _front = 0;
        _capacity = new_capacity;
    }
};