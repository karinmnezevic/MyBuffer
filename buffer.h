#include <iostream>
template <typename Buffer>
class BufferIterator {
public:
    using value_type = typename Buffer::value_type;
    using pointer = value_type*;
    using reference = value_type&;
    using difference_type = value_type;
    using iterator_category = std::random_access_iterator_tag;

private:
    pointer _data;

public:
    BufferIterator() = default;
    BufferIterator(pointer my_data) : 
        _data(my_data) {}
    BufferIterator& operator++() {
        _data ++;
        return *this; 
    }
    BufferIterator operator++(int) {
        BufferIterator ret = *this;
        ++(*this);
        return ret;
    }
    BufferIterator& operator--() {
        _data --;
        return *this;
    }
    BufferIterator operator--(int) {
        BufferIterator ret = *this;
        --(*this);
        return ret;
    }
    BufferIterator operator+(const difference_type other) const {
        return BufferIterator(_data + other);
    }
    BufferIterator operator-(const difference_type other) const {
        return BufferIterator(_data - other);
    }
    BufferIterator operator+(const BufferIterator& other) const {
        return BufferIterator(_data + other);
    }
    difference_type operator-(const BufferIterator& other) const { 
        return _data - other._data; 
    }
    BufferIterator& operator+=(const difference_type other) {
        _data += other;
        return *this;
    }
    BufferIterator& operator-=(const difference_type other) {
        _data -= other;
        return *this;
    }
    bool operator<(const BufferIterator& other) const {
        return _data < other._data;
    }
    bool operator<=(const BufferIterator& other) const {
        return _data <= other._data;
    }
    bool operator>(const BufferIterator& other) const {
        return _data < other._data;
    }
    bool operator>=(const BufferIterator& other) const {
        return _data >= other._data;
    }
    reference operator*() {
        return *_data;
    }
    pointer operator->() {
        return _data;
    }
    reference operator[](int index) {
        return *(_data + index);
    }
    bool operator==(const BufferIterator& other) const {
        return _data == other._data;
    }
    bool operator!=(BufferIterator other) const {
        return !(*this == other);
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
        ReAlloc(6);
    }

    ~Buffer() {
        Clear();
        ::operator delete(_mem_begin, _capacity * sizeof(T));
    }

    void PushBack(const T& val) {
        if(_data_begin + _size == _mem_begin + _capacity) //nema mjesta straga
            MakeRoom();

        new(&_data_begin[_size++]) T(val);
    }

    void PushBack(T&& val) {
        if(_data_begin + _size == _mem_begin + _capacity) //nema mjesta straga
            MakeRoom();
        
        new(&_data_begin[_size++]) T(std::move(val));
    }

    template<typename... Args>
    T& EmplaceBack(Args&&... args) {
        if(_data_begin + _size == _mem_begin + _capacity)
            MakeRoom();
        
        new(&_data_begin[_size]) T(std::forward<Args>(args)...); //placement new
        return _data_begin[_size++];
    }

    void PushFront(const T& val) {
        if(_data_begin == _mem_begin) //nema mjesta sprijeda
            MakeRoom();

        *(--_data_begin) = val;
        _size++;
    }

    void PopFront() {
        _data_begin[0].~T();
        _data_begin ++;
        _size --;
    }

    void PopBack() {
        _data_begin[--_size].~T();
    }

    void Clear() {
        for (size_t i = 0; i < _size; i++)
            _data_begin[i].~T();
        _size = 0;
    }

    size_t Size() const {
        return _size;
    }

    const T& operator[](size_t index) const {
        return _data_begin[index];
    }

    T& operator[](size_t index) {
        return _data_begin[index];
    }

    Iterator begin() {
        return Iterator(_data_begin);
    }

    Iterator end() {
        return Iterator(_data_begin + _size);
    }

private:
    void MakeRoom() {
        if (_size <= _capacity / 3) 
            MoveToMid();
        else
            ReAlloc(_capacity * 3);
    }

    void MoveToMid() {
        std::cout << "Moving to mid" << std::endl;
        for (size_t i = 0; i < _size; i ++)
           _mem_begin[i + _capacity/3] = _data_begin[i];
    }

    void ReAlloc(size_t new_capacity) {
        std::cout << "Reallocating " << new_capacity << std::endl;

        T* new_mem = (T*)::operator new(new_capacity * sizeof(T));
        for (size_t i = 0; i < _size; i ++)
            new(&new_mem[i]) T(std::move(_data_begin[i]));
        
        for (size_t i = 0; i < _size; i ++)
            _data_begin[i].~T();

        ::operator delete(_mem_begin, _capacity * sizeof(T));

        _mem_begin = new_mem;
        _data_begin = new_mem + new_capacity/3;
        _capacity = new_capacity;
    }
};
