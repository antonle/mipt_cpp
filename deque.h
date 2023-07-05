#include <iostream>

template <typename T>
class Deque {
private:
    template <bool is_const>
    class Iterator;

public:
    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    Deque();
    explicit Deque(size_t number_of_elements);
    Deque(size_t number_of_elements, const T& value);
    Deque(const Deque& other);
    Deque& operator=(const Deque& other);
    ~Deque();

    size_t size() const;

    T& operator[](size_t position);
    const T& operator[](size_t position) const;
    T& at(size_t position);
    const T& at(size_t position) const;

    void push_back(const T& value);
    void pop_back();
    void push_front(const T& value);
    void pop_front();

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
    reverse_iterator rbegin();
    reverse_iterator rend();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator rend() const;
    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;

    void insert(const iterator& it, const T& value);
    void erase(iterator it);

private:
    void SetSize();
    void IncreaseArray(size_t new_number_of_blocks);
    void DeleteArray();
    void CopyArray(const Deque& other);

    static const size_t size_of_array_ = 32;
    static const size_t expansion_koefficient_ = 3;
    static const size_t increase_number_ = 1;

    T** deque_;
    size_t first_element_index_;
    size_t number_of_blocks_;
    size_t deque_size_;
};

template <typename T>
template <bool is_const>
class Deque<T>::Iterator {
public:
    Iterator(typename std::conditional<is_const, T** const, T**>::type it_deque, size_t index);

    Iterator<is_const>& operator++() &;
    Iterator<is_const> operator++(int) &;
    Iterator<is_const>& operator--() &;
    Iterator<is_const> operator--(int) &;
    Iterator<is_const>& operator+=(int number);
    Iterator<is_const>& operator+(int number);
    Iterator<is_const>& operator-=(int number);
    Iterator<is_const>& operator-(int number);
    size_t operator-(const Iterator<is_const>& rhs);

    bool operator>(const Iterator<is_const>& rhs);
    bool operator<(const Iterator<is_const>& rhs);
    bool operator>=(const Iterator<is_const>& rhs);
    bool operator<=(const Iterator<is_const>& rhs);
    bool operator==(const Iterator<is_const>& rhs);
    bool operator!=(const Iterator<is_const>& rhs);

    typename std::conditional<is_const, const T&, T&>::type operator*();
    typename std::conditional<is_const, const T*, T*>::type operator->();

    size_t getIndex() const;

private:
    static const size_t size_of_array_ = 32;

    T** it_deque_;
    size_t index_;
};

template <typename T>
template <bool is_const>
Deque<T>::Iterator<is_const>::Iterator(typename std::conditional<is_const, T** const, T**>::type it_deque, size_t index): it_deque_(it_deque),
                                                                                                           index_(index) {}

template <typename T>
template <bool is_const>
typename Deque<T>::template Iterator<is_const>& Deque<T>::Iterator<is_const>::operator++() & {
     ++index_;
    return *this;
}

template <typename T>
template <bool is_const>
typename Deque<T>::template Iterator<is_const> Deque<T>::Iterator<is_const>::operator++(int) & {
    Iterator<is_const> it = *this;
    ++*this;
    return it;
}

template <typename T>
template <bool is_const>
typename Deque<T>::template Iterator<is_const>& Deque<T>::Iterator<is_const>::operator--() & {
     --index_;
    return *this;
}

template <typename T>
template <bool is_const>
typename Deque<T>::template Iterator<is_const> Deque<T>::Iterator<is_const>::operator--(int) & {
    Iterator<is_const> it = *this;
    --*this;
    return it;
}

template <typename T>
template <bool is_const>
typename Deque<T>::template Iterator<is_const>& Deque<T>::Iterator<is_const>::operator+=(int number) {
    index_ += number;
    return *this;
}

template <typename T>
template <bool is_const>
typename Deque<T>::template Iterator<is_const>& Deque<T>::Iterator<is_const>::operator+(int number) {
    *this += number;
    return *this;
}

template <typename T>
template <bool is_const>
typename Deque<T>::template Iterator<is_const>& Deque<T>::Iterator<is_const>::operator-=(int number) {
    index_ -= number;
    return *this;
}

template <typename T>
template <bool is_const>
typename Deque<T>::template Iterator<is_const>& Deque<T>::Iterator<is_const>::operator-(int number) {
    *this -= number;
    return *this;
}

template <typename T>
template <bool is_const>
size_t Deque<T>::Iterator<is_const>::operator-(const Iterator<is_const>& rhs) {
    return index_ - rhs.index_;
}

template <typename T>
template <bool is_const>
bool Deque<T>::Iterator<is_const>::operator>(const Iterator<is_const>& rhs) {
    return index_ > rhs.index_;
}

template <typename T>
template <bool is_const>
bool Deque<T>::Iterator<is_const>::operator<(const Iterator<is_const>& rhs) {
    return index_ < rhs.index_;
}

template <typename T>
template <bool is_const>
bool Deque<T>::Iterator<is_const>::operator==(const Iterator<is_const>& rhs) {
    return index_ ==  rhs.index_;
}

template <typename T>
template <bool is_const>
bool Deque<T>::Iterator<is_const>::operator>=(const Iterator<is_const>& rhs) {
    return index_ >= rhs.index_;
}

template <typename T>
template <bool is_const>
bool Deque<T>::Iterator<is_const>::operator<=(const Iterator<is_const>& rhs) {
    return index_ <= rhs.index_;
}

template <typename T>
template <bool is_const>
bool Deque<T>::Iterator<is_const>::operator!=(const Iterator<is_const>& rhs) {
    return index_ != rhs.index_;
}

template <typename T>
template <bool is_const>
typename std::conditional<is_const, const T&, T&>::type Deque<T>::Iterator<is_const>::operator*() {
    return it_deque_[index_ / size_of_array_][index_ % size_of_array_];
}

template <typename T>
template <bool is_const>
typename std::conditional<is_const, const T*, T*>::type Deque<T>::Iterator<is_const>::operator->() {
    return it_deque_[index_ / size_of_array_] + index_ % size_of_array_;
}

template <typename T>
template <bool is_const>
size_t Deque<T>::Iterator<is_const>::getIndex() const {
    return index_;
}

template <typename T>
Deque<T>::Deque(): first_element_index_(0),
                   number_of_blocks_(1),
                   deque_size_(0)
{
    SetSize();
}

template <typename T>
Deque<T>::Deque(size_t number_of_elements): first_element_index_(0),
                                  number_of_blocks_(number_of_elements / size_of_array_ + 1),
                                  deque_size_(0)
{
    SetSize();

    for (size_t i = 0; i < number_of_elements; ++i) {
        try {
            push_back(T());
        } catch(...) {
            pop_back();

            throw;
        }
    }
}

template <typename T>
Deque<T>::Deque(size_t number_of_elements, const T& value): first_element_index_(0),
                                                  number_of_blocks_(number_of_elements / size_of_array_ + 1),
                                                  deque_size_(0)
{
    SetSize();

    for (size_t i = 0; i < number_of_elements; ++i) {
        try {
            push_back(value);
        } catch(...) {
            pop_back();

            throw;
        }
    }
}

template <typename T>
Deque<T>::Deque(const Deque& other): first_element_index_(other.first_element_index_),
                           number_of_blocks_(other.number_of_blocks_),
                           deque_size_(other.deque_size_)
{
    CopyArray(other);
}

template <typename T>
typename Deque<T>::Deque& Deque<T>::operator=(const Deque& other) {
    if (deque_ == other.deque_) {
        return *this;
    }

    DeleteArray();

    CopyArray(other);

    first_element_index_ = other.first_element_index_;
    number_of_blocks_ = other.number_of_blocks_;
    deque_size_ = other.deque_size_;

    return *this;
}

template <typename T>
Deque<T>::~Deque() {
    DeleteArray();
}

template <typename T>
size_t Deque<T>::size() const {
    return deque_size_;
}

template <typename T>
T& Deque<T>::operator[](size_t position) {
    size_t index = first_element_index_ + position;
    return deque_[index / size_of_array_][index % size_of_array_];
}

template <typename T>
const T& Deque<T>::operator[](size_t position) const{
    size_t index = first_element_index_ + position;
    return deque_[index / size_of_array_][index % size_of_array_];
}

template <typename T>
T& Deque<T>::at(size_t position) {
    if (position >= deque_size_) {
        throw std::out_of_range("INDEX OUT OF RANGE");
    }

    size_t index = first_element_index_ + position;
    return deque_[index / size_of_array_][index % size_of_array_];
}

template <typename T>
const T& Deque<T>::at(size_t position) const {
    if (position >= deque_size_) {
        throw std::out_of_range("INDEX OUT OF RANGE");
    }

    size_t index = first_element_index_ + position;
    return deque_[index / size_of_array_][index % size_of_array_];
}

template <typename T>
void Deque<T>::push_back(const T& value) {
    if (first_element_index_ + deque_size_ == size_of_array_ * number_of_blocks_) {
        IncreaseArray(expansion_koefficient_ * number_of_blocks_);
    }

    size_t new_element_index = first_element_index_ + deque_size_;
    try {
        new(deque_[new_element_index / size_of_array_] + new_element_index % size_of_array_) T(value);
        ++deque_size_;
    } catch(...) {
        deque_[new_element_index / size_of_array_][new_element_index % size_of_array_].~T();

        throw;
    }
}

template <typename T>
void Deque<T>::pop_back() {
    size_t last_element_index = first_element_index_ + deque_size_ - 1;
    (deque_[last_element_index / size_of_array_] + last_element_index % size_of_array_)->~T();

    --deque_size_;
}

template <typename T>
void Deque<T>::push_front(const T& value) {
    if (first_element_index_ == 0) {
        IncreaseArray(expansion_koefficient_ * number_of_blocks_);
    }

    try {
        ++deque_size_;
        --first_element_index_;
        new(deque_[first_element_index_ / size_of_array_] + first_element_index_ % size_of_array_) T(value);
    } catch(...) {
        deque_[first_element_index_ / size_of_array_][first_element_index_ % size_of_array_].~T();

        throw;
    }
}

template <typename T>
void Deque<T>::pop_front() {
    (deque_[first_element_index_ / size_of_array_] + first_element_index_ % size_of_array_)->~T();

    ++first_element_index_;
    --deque_size_;
}

template <typename T>
typename Deque<T>::iterator Deque<T>::begin() {
    return iterator(deque_, first_element_index_);
}

template <typename T>
typename Deque<T>::iterator Deque<T>::end() {
    iterator it(deque_, first_element_index_);
    it += deque_size_;
    return it;
}

template <typename T>
typename Deque<T>::const_iterator Deque<T>::begin() const {
    return const_iterator(deque_, first_element_index_);
}

template <typename T>
typename Deque<T>::const_iterator Deque<T>::end() const {
    const_iterator it(deque_, first_element_index_);
    it += deque_size_;
    return it;
}

template <typename T>
typename Deque<T>::const_iterator Deque<T>::cbegin() const {
    return const_iterator(deque_, first_element_index_);
}

template <typename T>
typename Deque<T>::const_iterator Deque<T>::cend() const {
    const_iterator it(deque_, first_element_index_);
    it += deque_size_;
    return it;
}

template <typename T>
typename Deque<T>::reverse_iterator Deque<T>::rbegin() {
    return reverse_iterator(end());
}

template <typename T>
typename Deque<T>::reverse_iterator Deque<T>::rend() {
    return reverse_iterator(begin());
}

template <typename T>
typename Deque<T>::const_reverse_iterator Deque<T>::rbegin() const {
    return const_reverse_iterator(cend());
}

template <typename T>
typename Deque<T>::const_reverse_iterator Deque<T>::rend() const {
    return const_reverse_iterator(cbegin());
}

template <typename T>
typename Deque<T>::const_reverse_iterator Deque<T>::crbegin() const {
    return const_reverse_iterator(cend());
}

template <typename T>
typename Deque<T>::const_reverse_iterator Deque<T>::crend() const {
    return const_reverse_iterator(cbegin());
}

template <typename T>
void Deque<T>::insert(const iterator& it, const T& value) {
   if (first_element_index_ + deque_size_ == size_of_array_ * number_of_blocks_) {
        IncreaseArray(expansion_koefficient_ * number_of_blocks_);
   }

   size_t index = deque_size_ + first_element_index_;
   for (size_t i = index; i > it.getIndex(); --i) {
       deque_[i / size_of_array_][i % size_of_array_] = \
       deque_[(i - increase_number_) / size_of_array_][(i - increase_number_) % size_of_array_];
   }

   deque_[it.getIndex() / size_of_array_][it.getIndex() % size_of_array_] = value;
   ++deque_size_;
}

template <typename T>
void Deque<T>::erase(iterator it) {
    for (size_t i = it.getIndex(); i < first_element_index_ + deque_size_ - increase_number_; ++i) {
        deque_[i / size_of_array_][i % size_of_array_] = \
        deque_[(i + increase_number_) / size_of_array_][(i + increase_number_) % size_of_array_];
    }

    size_t index = first_element_index_ + deque_size_ - increase_number_;
    deque_[index / size_of_array_][index % size_of_array_].~T();
    --deque_size_;
}

template <typename T>
void Deque<T>::SetSize() {
    deque_ = new T*[number_of_blocks_];

    for (size_t i = 0; i < number_of_blocks_; ++i) {
        deque_[i] = reinterpret_cast<T*>(new uint8_t[size_of_array_ * sizeof(T)]);
    }
}

template <typename T>
void Deque<T>::IncreaseArray(size_t new_number_of_blocks) {
    T** new_deque = new T*[new_number_of_blocks];

    size_t new_first_element_index = first_element_index_ + number_of_blocks_ * size_of_array_;
    for (size_t i = 0; i < new_number_of_blocks; ++i) {
        if (i >= number_of_blocks_ && i < 2 * number_of_blocks_) {
            new_deque[i] = deque_[i - number_of_blocks_];
        }
        else {
            new_deque[i] = reinterpret_cast<T*>(new uint8_t[size_of_array_ * sizeof(T)]);
        }
    }

    delete[] deque_;

    deque_ = new_deque;
    first_element_index_ = new_first_element_index;
    number_of_blocks_ = new_number_of_blocks;
}

template <typename T>
void Deque<T>::DeleteArray() {
    for (size_t i = first_element_index_; i < deque_size_ + first_element_index_; ++i) {
        (deque_[i / size_of_array_] + i % size_of_array_)->~T();
    }

    for (size_t i = 0; i < number_of_blocks_; ++i) {
        delete[] reinterpret_cast<uint8_t*>(deque_[i]);
    }

    delete[] deque_;
}

template <typename T>
void Deque<T>::CopyArray(const Deque& other) {
    deque_ = new T*[other.number_of_blocks_];

    for (size_t i = 0; i < other.number_of_blocks_; ++i) {
        deque_[i] = reinterpret_cast<T*>(new uint8_t[size_of_array_ * sizeof(T)]);
    }

    for (size_t i = other.first_element_index_; i < other.first_element_index_ + other.deque_size_; ++i) {
        new(deque_[i / size_of_array_] + i % size_of_array_) T(other.deque_[i / size_of_array_][i % size_of_array_]);
    }
}
