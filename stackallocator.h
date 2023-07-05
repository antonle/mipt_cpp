#include <iostream>
#include <iterator>

template <size_t N>
class StackStorage {
public:
    StackStorage();
    StackStorage(const StackStorage& other) = delete;
    uint8_t* allocate(size_t shift, size_t number_of_memory);
    size_t getAddress() const;

private:
    uint8_t memory_ [N];
    uint8_t* pointer_to_free_memory_;
};

template <size_t N>
StackStorage<N>::StackStorage(): pointer_to_free_memory_(memory_) {}

template <size_t N>
uint8_t* StackStorage<N>::allocate(size_t shift, size_t number_of_memory) {
    uint8_t* pointer_to_chunk = pointer_to_free_memory_ + shift;
    pointer_to_free_memory_ += number_of_memory + shift;
    return pointer_to_chunk;
}

template <size_t N>
size_t StackStorage<N>::getAddress() const {
    return reinterpret_cast<size_t>(pointer_to_free_memory_);
}

template <typename T, size_t N>
class StackAllocator {
public:
    using value_type = T;

    explicit StackAllocator(StackStorage<N>& memory);

    template <typename U>
    StackAllocator(const StackAllocator<U, N>& other);

    template <typename U>
    StackAllocator<T, N>& operator=(const StackAllocator<U, N>& other);

    T* allocate(size_t number_of_elements);
    void deallocate(T*, size_t);

    template <typename U>
    struct rebind;

    template <typename U>
    bool operator==(const StackAllocator<U, N>& rhs);

    template <typename U>
    bool operator!=(const StackAllocator<U, N>& lhs);

    StackStorage<N>* getStorage() const;

private:
    StackStorage<N>* memory_;
};

template <typename T, size_t N>
StackAllocator<T, N>::StackAllocator(StackStorage<N>& memory): memory_(&memory){}

template <typename T, size_t N>
template <typename U>
StackAllocator<T, N>::StackAllocator(const StackAllocator<U, N>& other): memory_(other.getStorage()) {}

template <typename T, size_t N>
template <typename U>
StackAllocator<T, N>& StackAllocator<T, N>::operator=(const StackAllocator<U, N>& other) {
    memory_ = other.getStorage();
    return *this;
}

template <typename T, size_t N>
T* StackAllocator<T, N>::allocate(size_t number_of_elements) {
    size_t modulo = (alignof(T) - memory_->getAddress() % alignof(T)) % alignof(T);
    alignas(alignof(T)) T* chunk_of_memory = reinterpret_cast<T*>(memory_->allocate(modulo, number_of_elements * sizeof(T)));
    return chunk_of_memory;
}

template <typename T, size_t N>
void StackAllocator<T, N>::deallocate(T*, size_t) {}

template <typename T, size_t N>
template <typename U>
struct StackAllocator<T, N>::rebind {
    using other = StackAllocator<U, N>;
};

template <typename T, size_t N>
template <typename U>
bool StackAllocator<T, N>::operator==(const StackAllocator<U, N>& rhs) {
    return memory_ == rhs.getStorage();
}

template <typename T, size_t N>
template <typename U>
bool StackAllocator<T, N>::operator!=(const StackAllocator<U, N>& rhs) {
    return !(*this == rhs);
}

template <typename T, size_t N>
StackStorage<N>* StackAllocator<T, N>::getStorage() const {
    return memory_;
}

template <typename T, typename Allocator = std::allocator<T>>
class List {
private:
    struct Node;

    template <bool is_const>
    class Iterator;

public:
    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using alloc_traits = std::allocator_traits<typename Allocator::template rebind<Node>::other>;

    List();
    explicit List(size_t number_of_nodes);
    List(size_t number_of_nodes, const T& value);
    explicit List(const Allocator& allocator);
    List(size_t number_of_nodes, const Allocator& allocator);
    List(size_t number_of_nodes, const T& value, const Allocator& allocator);
    List(const List& other);
    ~List();
    List& operator=(const List& other);
    typename Allocator::template rebind<Node>::other get_allocator();

    size_t size() const;

    template <typename... Args>
    void push_back(const Args&... args);

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

    void insert(const_iterator it, const T& value);
    void erase(const_iterator it);

    bool operator==(const List& other);

private:
    void setHead();
    void setList(size_t number_of_nodes, const T& value);

    void destroyList();

    void addEmptyElements(size_t number_of_nodes);
    void addNotEmptyElements(size_t number_of_nodes, const T& value);

    size_t size_;
    typename Allocator::template rebind<Node>::other allocator_;
    Node* head_;
};

template <typename T, typename Allocator>
struct List<T, Allocator>::Node {
    T value;
    Node* previous;
    Node* next;

    Node() = default;

    explicit Node(const T& value): value(value), previous(nullptr), next(nullptr) {}
};

template <typename T, typename Allocator>
template <bool is_const>
class List<T, Allocator>::Iterator {
public:
    using difference_type = long long;
    using value_type = typename std::conditional<is_const, const T, T>::type;
    using pointer = typename std::conditional<is_const, const T*, T*>::type;
    using reference = typename std::conditional<is_const, const T&, T&>::type;
    using iterator_category = std::bidirectional_iterator_tag;

    explicit Iterator(typename std::conditional<is_const, Node* const, Node*>::type head);
    Iterator(const Iterator<false>& it);
    Iterator& operator=(const Iterator<false>& it);

    Iterator<is_const>& operator++() &;
    Iterator<is_const> operator++(int) &;
    Iterator<is_const>& operator--() &;
    Iterator<is_const> operator--(int) &;

    typename std::conditional<is_const, const T&, T&>::type operator*() const;
    typename std::conditional<is_const, const T*, T*>::type operator->() const;

    Node* getNode() const;

    bool operator==(const Iterator<is_const>& rhs) const;
    bool operator !=(const Iterator<is_const>& rhs) const;

private:
    Node* it_head_;
};

template <typename T, typename Allocator>
template <bool is_const>
List<T, Allocator>::Iterator<is_const>::Iterator(typename std::conditional<is_const, Node* const, Node*>::type head): it_head_(head) {}

template <typename T, typename Allocator>
template <bool is_const>
List<T, Allocator>::Iterator<is_const>::Iterator(const Iterator<false>& it): it_head_(it.getNode()) {}

template <typename T, typename Allocator>
template <bool is_const>
typename List<T, Allocator>::template Iterator<is_const>& List<T, Allocator>::Iterator<is_const>::operator=(const Iterator<false>& it) {
    it_head_ = it.getNode();
    return *this;
}

template <typename T, typename Allocator>
template <bool is_const>
typename List<T, Allocator>::template Iterator<is_const>& List<T, Allocator>::Iterator<is_const>::operator++() & {
    it_head_ = it_head_->next;
    return *this;
}

template <typename T, typename Allocator>
template <bool is_const>
typename List<T, Allocator>::template Iterator<is_const> List<T, Allocator>::Iterator<is_const>::operator++(int) & {
    Iterator<is_const> it = *this;
    ++*this;
    return it;
}

template <typename T, typename Allocator>
template <bool is_const>
typename List<T, Allocator>::template Iterator<is_const>& List<T, Allocator>::Iterator<is_const>::operator--() & {
    it_head_ = it_head_->previous;
    return *this;
}

template <typename T, typename Allocator>
template <bool is_const>
typename List<T, Allocator>::template Iterator<is_const> List<T, Allocator>::Iterator<is_const>::operator--(int) & {
    Iterator<is_const> it = *this;
    --*this;
    return it;
}

template <typename T, typename Allocator>
template <bool is_const>
typename std::conditional<is_const, const T&, T&>::type List<T, Allocator>::Iterator<is_const>::operator*() const {
    return it_head_->value;
}

template <typename T, typename Allocator>
template <bool is_const>
typename std::conditional<is_const, const T*, T*>::type List<T, Allocator>::Iterator<is_const>::operator->() const{
    return &it_head_->value;
}

template <typename T, typename Allocator>
template <bool is_const>
typename List<T, Allocator>::Node* List<T, Allocator>::Iterator<is_const>::getNode() const {
    return it_head_;
}

template <typename T, typename Allocator>
template <bool is_const>
bool List<T, Allocator>::Iterator<is_const>::operator==(const Iterator<is_const>& rhs) const {
    return it_head_ == rhs.it_head_;
}

template <typename T, typename Allocator>
template <bool is_const>
bool List<T, Allocator>::Iterator<is_const>::operator!=(const Iterator<is_const>& rhs) const {
    return !(*this == rhs);
}

template <typename T, typename Allocator>
List<T, Allocator>::List(): size_(0) {
    setHead();
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t number_of_nodes): size_(0) {
    setHead();

    addEmptyElements(number_of_nodes);
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t number_of_nodes, const T& value): size_(0) {
    setHead();

    addNotEmptyElements(number_of_nodes, value);
}

template <typename T, typename Allocator>
List<T, Allocator>::List(const Allocator& allocator): size_(0),
                allocator_(alloc_traits::select_on_container_copy_construction(allocator))
{
    setHead();
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t number_of_nodes, const Allocator& allocator): List(allocator) {
    addEmptyElements(number_of_nodes);
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t number_of_nodes, const T& value, const Allocator& allocator): List(allocator) {
    addNotEmptyElements(number_of_nodes, value);
}

template <typename T, typename Allocator>
List<T, Allocator>::List(const List& other): List(other.allocator_) {
    Node* node = other.head_->next;
    for (size_t i = 0; i < other.size_; ++i) {
        try {
            push_back(node->value);
            node = node->next;
        } catch(...) {
            for (size_t j = 0; j < i; ++j) {
                pop_back();
            }
            throw;
        }
    }
}

template <typename T, typename Allocator>
List<T, Allocator>::~List() {
    destroyList();
}

template <typename T, typename Allocator>
typename List<T, Allocator>::List& List<T, Allocator>::operator=(const List& other) {
    if (*this == other) {
        return *this;
    }

    List<T, Allocator> copy = other;

    if (alloc_traits::propagate_on_container_copy_assignment::value) {
        allocator_ = other.allocator_;
    }
    else {
        allocator_ = alloc_traits::select_on_container_copy_construction(other.allocator_);
    }

    std::swap(head_, copy.head_);
    std::swap(size_, copy.size_);

    return *this;
}

template <typename T, typename Allocator>
typename Allocator::template rebind<typename List<T, Allocator>::Node>::other List<T, Allocator>::get_allocator() {
    return allocator_;
}

template <typename T, typename Allocator>
size_t List<T, Allocator>::size() const {
    return size_;
}

template <typename T, typename Allocator>
template <typename... Args>
void List<T, Allocator>::push_back(const Args&... args) {
    try {
        Node* node = allocator_.allocate(1);

        alloc_traits::construct(allocator_, node, args...);

        node->next = head_;
        node->previous = head_->previous;
        head_->previous->next = node;
        head_->previous = node;

        ++size_;
    } catch(...) {
        throw;
    }
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_back() {
    Node* node = head_->previous;

    head_->previous = node->previous;
    node->previous->next  = head_;

    node->value.~T();

    --size_;

    alloc_traits::deallocate(allocator_, node, 1);
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_front(const T& value) {
    try {
        Node* node = allocator_.allocate(1);

        node->value = value;
        node->previous = head_;
        node->next = head_->next;
        head_->next->previous = node;
        head_->next = node;

        ++size_;
    } catch (...) {
        throw;
    }
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_front() {
    Node* node = head_->next;

    head_->next = node->next;
    node->next->previous = head_;

    node->value.~T();

    --size_;

    alloc_traits::deallocate(allocator_, node, 1);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::begin() {
    return iterator(head_->next);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::end() {
    return iterator(head_);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::begin() const {
    return const_iterator(head_->next);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::end() const {
    return const_iterator(head_);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::cbegin() const {
    return const_iterator(head_->next);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::cend() const {
    return const_iterator(head_);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::reverse_iterator List<T, Allocator>::rbegin() {
    return reverse_iterator(iterator(head_));
}

template <typename T, typename Allocator>
typename List<T,Allocator>::reverse_iterator List<T,Allocator>::rend() {
    return reverse_iterator(iterator(head_->next));
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::rbegin() const {
    return const_reverse_iterator(const_iterator(head_));
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::rend() const {
    return const_reverse_iterator(const_iterator(head_->next));
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::crbegin() const {
    return const_reverse_iterator(const_iterator(head_));
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::crend() const {
    return const_reverse_iterator(const_iterator(head_->next));
}

template <typename T, typename Allocator>
void List<T, Allocator>::insert(const_iterator it, const T& value) {
    Node* node = allocator_.allocate(1);

    Node* current_node = it.getNode();

    node->value = value;
    node->next = current_node;
    node->previous = current_node->previous;

    current_node->previous->next = node;
    current_node->previous = node;

    ++size_;
}

template <typename T, typename Allocator>
void List<T, Allocator>::erase(const_iterator it) {
    Node* node = it.getNode();

    node->next->previous = node->previous;
    node->previous->next = node->next;

    node->value.~T();

    --size_;

    allocator_.deallocate(node, 1);
}

template <typename T, typename Allocator>
bool List<T, Allocator>::operator==(const List& other) {
    return head_ == other.head_;
}

template <typename T, typename Allocator>
void List<T, Allocator>::setHead() {
    head_ = allocator_.allocate(1);

    head_->next = head_;
    head_->previous = head_;
}

template <typename T, typename Allocator>
void List<T, Allocator>::setList(size_t number_of_nodes, const T& value) {
    setHead();

    for (size_t i = 0; i < number_of_nodes; ++i) {
        push_back(value);
    }
}

template <typename T, typename Allocator>
void List<T, Allocator>::destroyList() {
    Node* node = head_->next;
    for (size_t i = 0; i < size_; ++i) {
        Node* next = node->next;
        node->value.~T();
        alloc_traits::deallocate(allocator_, node, 1);
        node = next;
    }

    alloc_traits::deallocate(allocator_, head_, 1);
}

template <typename T, typename Allocator>
void List<T, Allocator>::addEmptyElements(size_t number_of_nodes) {
    for (size_t i = 0; i < number_of_nodes; ++i) {
        try {
            push_back();
        } catch(...) {
            for (size_t j = 0; j < i; ++j) {
                pop_back();
            }
            throw;
        }
    }
}

template <typename T, typename Allocator>
void List<T, Allocator>::addNotEmptyElements(size_t number_of_nodes, const T& value) {
    for (size_t i = 0; i < number_of_nodes; ++i) {
        try {
            push_back(value);
        } catch(...) {
            for (size_t j = 0; j < i; ++j) {
                pop_back();
            }
            throw;
        }
    }
}
