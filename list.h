#pragma once
#include <cstddef>
#include <iostream>
#include <memory>

template <typename T, typename Alloc = std::allocator<T>>
class List {
  private:
    struct BaseNode {
        BaseNode* next;
        BaseNode* prev;

        BaseNode()
            : next(this), prev(this) {}
        BaseNode(BaseNode* nxt, BaseNode* prv)
            : next(nxt), prev(prv) {}
    };

    struct Node : BaseNode {
        T value;
        Node(BaseNode* nxt, BaseNode* prv, const T& val)
            : BaseNode(nxt, prv), value(val) {}
        Node(BaseNode* nxt, BaseNode* prv)
            : BaseNode(nxt, prv), value(T()) {}
    };

    using AllocTraits = std::allocator_traits<Alloc>;
    using NodeAlloc = typename AllocTraits::template rebind_alloc<Node>;
    using NodeAllocTraits = std::allocator_traits<NodeAlloc>;

    BaseNode _fake_node;
    size_t _size;
    NodeAlloc _node_alloc;

    void swap(List& other) {
        std::swap(_fake_node.prev, other._fake_node.prev);
        std::swap(_fake_node.next, other._fake_node.next);
        std::swap(_fake_node.next->prev, other._fake_node.next->prev);
        std::swap(_fake_node.prev->next, other._fake_node.prev->next);
        std::swap(_size, other._size);
    }

  public:
    template <bool is_const>
    class CommonIterator {
      public:
        using value_type = T;
        using reference_t = std::conditional_t<is_const, const T&, T&>;
        using pointer_node = std::conditional_t<is_const, const BaseNode*, BaseNode*>;
        using pointer_t = std::conditional_t<is_const, const T*, T*>;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        CommonIterator() = default;
        CommonIterator(const CommonIterator&) = default;
        CommonIterator& operator=(const CommonIterator&) = default;

        reference_t operator*() const {
            return static_cast<Node*>(_ptr_node)->value;
        }

        CommonIterator& operator++() {
            _ptr_node = _ptr_node->next;
            return *this;
        };

        CommonIterator operator++(int) {
            CommonIterator copy = *this;
            _ptr_node = _ptr_node->next;
            return copy;
        }

        CommonIterator& operator--() {
            _ptr_node = _ptr_node->prev;
            return *this;
        };

        CommonIterator operator--(int) {
            CommonIterator copy = *this;
            _ptr_node = _ptr_node->prev;
            return copy;
        }

        bool operator==(const CommonIterator& other) const {
            return _ptr_node == other.getPtrNode();
        }

        bool operator!=(const CommonIterator& other) const {
            return !(operator==(other));
        }

        operator CommonIterator<true>() const {
            return CommonIterator<true>(_ptr_node);
        }

      private:
        BaseNode* _ptr_node;

        CommonIterator(BaseNode* ptr)
            : _ptr_node(ptr) {}
        BaseNode* getPtrNode() const {
            return _ptr_node;
        }
        friend List;
    };

    using iterator = CommonIterator<false>;
    using const_iterator = CommonIterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() {
        return iterator(_fake_node.next);
    }

    const_iterator begin() const {
        return const_iterator(_fake_node.next);
    }

    const_iterator cbegin() const {
        return const_iterator(_fake_node.next);
    }

    iterator end() {
        return iterator(&_fake_node);
    }

    const_iterator cend() const {
        return const_iterator(_fake_node.next->prev);
    }

    const_iterator end() const {
        return cend();
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator rbegin() const {
        return crbegin();
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator(cbegin());
    }

    reverse_iterator rend() const {
        return crend();
    }

    template <typename... Args>
    iterator insert(const_iterator it, const Args&... args) {
        BaseNode* prev = it.getPtrNode()->prev;
        BaseNode* next = it.getPtrNode();
        Node* ptr = NodeAllocTraits::allocate(_node_alloc, 1);
        try {
            NodeAllocTraits::construct(_node_alloc, ptr, next, prev, args...);
        } catch (...) {
            NodeAllocTraits::deallocate(_node_alloc, ptr, 1);
            throw;
        }
        prev->next = ptr;
        next->prev = ptr;
        ++_size;
        return iterator(static_cast<BaseNode*>(ptr));
    }

    iterator erase(const_iterator it) {
        BaseNode* prev = it.getPtrNode()->prev;
        BaseNode* next = it.getPtrNode()->next;
        prev->next = next;
        next->prev = prev;
        NodeAllocTraits::destroy(_node_alloc, static_cast<Node*>(it.getPtrNode()));
        NodeAllocTraits::deallocate(_node_alloc, static_cast<Node*>(it.getPtrNode()), 1);
        --_size;
        return iterator(static_cast<BaseNode*>(next));
    }

    void push_back(const T& value) {
        insert(end(), value);
    }

    void push_front(const T& value) {
        insert(begin(), value);
    }

    void pop_back() {
        erase(--end());
    }

    void pop_front() {
        erase(begin());
    }

    size_t size() const {
        return _size;
    }

    bool empty() const {
        return _size == 0;
    }

    List(const NodeAlloc& alloc = Alloc())
        : _size(0), _node_alloc(alloc) {}

    List(size_t count, const NodeAlloc& alloc = Alloc())
        : _size(0), _node_alloc(alloc) {
        size_t created = 0;

        try {
            for (; created < count; ++created) {
                insert(end());
            }
        } catch (...) {
            for (size_t i = 0; i < created; ++i) {
                pop_back();
            }
            throw;
        }
    }

    List(size_t count, const T& value, const NodeAlloc& alloc = Alloc())
        : _size(0), _node_alloc(alloc) {
        size_t created = 0;

        try {
            for (; created < count; ++created) {
                push_back(value);
            }
        } catch (...) {
            for (size_t i = 0; i < created; ++i) {
                pop_back();
            }
            throw;
        }
    }

    List(const List& other)
        : _size(0) {
        _node_alloc = NodeAllocTraits::select_on_container_copy_construction(other._node_alloc);
        size_t created = 0;

        try {
            for (const auto& value : other) {
                push_back(value);
                ++created;
            }
        } catch (...) {
            for (size_t i = 0; i < created; ++i) {
                pop_back();
            }
            throw;
        }
    }

    List& operator=(const List& other) {
        if (this == &other) {
            return *this;
        }

        List<T, Alloc> copy(other);
        if (size() == 0) {
            _fake_node.next = copy._fake_node.next;
            _fake_node.prev = copy._fake_node.prev;
            copy._fake_node.next->prev = &_fake_node;
            copy._fake_node.prev->next = &_fake_node;

            copy._fake_node.prev = &copy._fake_node;
            copy._fake_node.next = &copy._fake_node;
            std::swap(_size, copy._size);
        } else if (copy.size() == 0) {
            copy._fake_node.next = _fake_node.next;
            copy._fake_node.prev = _fake_node.prev;
            _fake_node.next->prev = &copy._fake_node;
            _fake_node.prev->next = &copy._fake_node;

            _fake_node.next = &_fake_node;
            _fake_node.prev = &_fake_node;
            std::swap(_size, copy._size);
        } else {
            swap(copy);
        }
        if (NodeAllocTraits::propagate_on_container_copy_assignment::value) {
            _node_alloc = other._node_alloc;
        }
        return *this;
    }

    Alloc get_allocator() {
        return _node_alloc;
    }

    ~List() {
        while (!empty()) {
            pop_back();
        }
    }
};
