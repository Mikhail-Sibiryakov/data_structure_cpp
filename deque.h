#include <algorithm>
#include <cstdio>
#include <iostream>

const size_t BUCKET_SIZE = 32;
const size_t DEFAULT_SIZE_EXT = 10;
const size_t NOTHING = BUCKET_SIZE * 2;

template <typename T>
class Deque {
  private:
    struct Node {
        T* ptr = nullptr;
        size_t begin = NOTHING;
        size_t end = NOTHING;
    };

    mutable size_t external_size{};
    mutable Node* external_array;
    mutable size_t front_pos{};
    mutable size_t back_pos{};
    mutable size_t _size{};

    void increase_external_array() const {
        size_t real_size = back_pos - front_pos + 1;
        Node* new_ext_arr = new Node[real_size * 3];
        for (size_t i = 0; i < external_size; ++i) {
            if (i <= std::min(back_pos, external_size - 1)) {
                new_ext_arr[real_size + (i - front_pos)] = external_array[i];
            } else if (external_array[i].ptr != nullptr) {
                delete[] reinterpret_cast<char*>(external_array[i].ptr);
            }
        }
        delete[] external_array;
        external_array = new_ext_arr;
        external_size = real_size * 3;
        front_pos = real_size;
        back_pos = real_size * 2 - 1;
    }

    void swap(Deque<T>& deque) {
        std::swap(external_size, deque.external_size);
        std::swap(external_array, deque.external_array);
        std::swap(front_pos, deque.front_pos);
        std::swap(back_pos, deque.back_pos);
        std::swap(_size, deque._size);
    }

  public:
    template <bool isConst>
    class CommonIterator {
      public:
        using value_type = T;
        using reference_t = std::conditional_t<isConst, const T&, T&>;
        using pointer_node = std::conditional_t<isConst, const Node*, Node*>;
        using pointer_t = std::conditional_t<isConst, const T*, T*>;
        using iterator_category = std::random_access_iterator_tag;

        CommonIterator() = default;

        CommonIterator(const CommonIterator&) = default;

        CommonIterator& operator=(const CommonIterator&) = default;

        CommonIterator& operator+=(std::ptrdiff_t x) {
            if (x + index < BUCKET_SIZE) {
                index += x;
                ptr_element += x;
            } else {
                x -= (BUCKET_SIZE - index - 1);
                size_t cur = (x % BUCKET_SIZE == 0 ? x / BUCKET_SIZE
                                                   : x / BUCKET_SIZE + 1);
                number_node += cur;
                ptr_node += cur;
                index = (x % BUCKET_SIZE == 0 ? BUCKET_SIZE - 1
                                              : x % BUCKET_SIZE - 1);
                ptr_element = ptr_node->ptr + index;
            }
            return *this;
        }

        CommonIterator& operator-=(std::ptrdiff_t x) {
            if (static_cast<int>(index) - static_cast<int>(x) >= 0) {
                index -= x;
                ptr_element -= x;
            } else {
                x -= index;
                number_node -= (x - 1) / BUCKET_SIZE + 1;
                ptr_node -= (x - 1) / BUCKET_SIZE + 1;
                size_t tmp = (x - ((x - 1) / BUCKET_SIZE) * BUCKET_SIZE);
                index = BUCKET_SIZE - tmp;
                ptr_element = ptr_node->ptr + index;
            }
            return *this;
        }

        reference_t operator*() {
            return *(ptr_element);
        }

        pointer_t operator->() const {
            return ptr_element;
        }

        CommonIterator& operator++() {
            return operator+=(1);
        }

        CommonIterator& operator--() {
            return operator-=(1);
        }

        CommonIterator operator++(int) {
            auto copy = *this;
            operator+=(1);
            return copy;
        }

        CommonIterator operator--(int) {
            auto copy = *this;
            operator-=(1);
            return copy;
        }

        ptrdiff_t operator-(const CommonIterator& other) const {
            std::ptrdiff_t number_a =
                this->number_node * BUCKET_SIZE + this->index + 1;
            std::ptrdiff_t number_b =
                other.number_node * BUCKET_SIZE + other.index + 1;
            return number_a - number_b;
        }

        bool operator==(const CommonIterator& other) const {
            return (ptr_node == other.ptr_node && index == other.index);
        }

        bool operator!=(const CommonIterator& other) const {
            return !operator==(other);
        }

        bool operator<(const CommonIterator& other) const {
            if (number_node == other.number_node) {
                return index < other.index;
            }
            return number_node < other.number_node;
        }

        bool operator>(const CommonIterator& other) const {
            return other < *this;
        }

        bool operator<=(const CommonIterator& other) const {
            return !operator>(other);
        }

        bool operator>=(const CommonIterator& other) const {
            return !operator<(other);
        }

        CommonIterator operator+(int diff) const {
            CommonIterator res = *this;
            if (diff >= 0) {
                res += diff;
            } else {
                res -= -diff;
            }
            return res;
        }

        CommonIterator operator-(int diff) const {
            return operator+(-diff);
        }

        operator CommonIterator<true>() {
            return CommonIterator<false>(ptr_node, index, number_node);
        }

      private:
        pointer_node ptr_node;
        size_t index;
        size_t number_node;
        pointer_t ptr_element;

        explicit CommonIterator(pointer_node ptr, size_t i, size_t n,
                                pointer_t ptr_el)
            : ptr_node(ptr), index(i), number_node(n), ptr_element(ptr_el) {}
        friend Deque;
    };

    using iterator = CommonIterator<false>;
    using const_iterator = CommonIterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() {
        size_t begin = external_array[front_pos].begin;
        return iterator(external_array + front_pos, begin, front_pos,
                        external_array[front_pos].ptr + begin);
    }

    Deque<T>& operator=(Deque<T> deque) {
        swap(deque);
        return *this;
    }

    const_iterator cbegin() const {
        size_t begin = external_array[front_pos].begin;
        return const_iterator(external_array + front_pos, begin, front_pos,
                              external_array[front_pos].ptr + begin);
    }

    const_iterator begin() const {
        return cbegin();
    }

    iterator end() {
        if (back_pos + 1 == external_size &&
            external_array[back_pos].end == BUCKET_SIZE - 1) {
            increase_external_array();
        }
        if (external_array[back_pos].end == BUCKET_SIZE - 1) {
            return iterator(external_array + back_pos + 1, 0, back_pos + 1,
                            external_array[back_pos + 1].ptr);
        }
        if (_size == 0) {
            return begin();
        }
        size_t end = external_array[back_pos].end;
        return iterator(external_array + back_pos, end + 1, back_pos,
                        external_array[back_pos].ptr + end + 1);
    }

    const_iterator cend() const {
        if (back_pos + 1 == external_size &&
            external_array[back_pos].end == BUCKET_SIZE - 1) {
            increase_external_array();
        }
        if (external_array[back_pos].end == BUCKET_SIZE - 1) {
            return const_iterator(external_array + back_pos + 1, 0,
                                  back_pos + 1,
                                  external_array[back_pos + 1].ptr);
        }
        if (_size == 0) {
            return begin();
        }
        size_t end = external_array[back_pos].end;
        return const_iterator(external_array + back_pos, end + 1, back_pos,
                              external_array[back_pos].ptr + end + 1);
    }

    const_iterator end() const {
        return cend();
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator(cbegin());
    }

    const_reverse_iterator rbegin() const {
        return crbegin();
    }

    const_reverse_iterator rend() const {
        return crend();
    }

    explicit Deque()
        : external_size(DEFAULT_SIZE_EXT),
          external_array(new Node[external_size]),
          front_pos(external_size / 2),
          back_pos(external_size / 2),
          _size(0) {
        external_array[front_pos].begin = 0;
        external_array[front_pos].end = 0;
    }

    Deque(const Deque<T>& deque)
        : external_size(deque.external_size),
          external_array(new Node[external_size]),
          front_pos(deque.front_pos),
          back_pos(deque.back_pos),
          _size(deque._size) {
        size_t i = front_pos;
        size_t j = 0;
        if (_size == 0) {
            return;
        }
        try {
            for (; i <= back_pos; ++i) {
                external_array[i].ptr =
                    reinterpret_cast<T*>(new char[BUCKET_SIZE * sizeof(T)]);
                external_array[i].begin = deque.external_array[i].begin;
                external_array[i].end = deque.external_array[i].end;
                for (j = external_array[i].begin; j <= external_array[i].end;
                     ++j) {
                    new (external_array[i].ptr + j)
                        T(deque.external_array[i].ptr[j]);
                }
            }
        } catch (...) {
            for (size_t k = front_pos; k <= i; ++k) {
                for (size_t l = external_array[k].begin;
                     l + 1 <= (k != i ? external_array[k].end + 1 : j); ++l) {
                    (external_array[k].ptr + l)->~T();
                }
                delete[] reinterpret_cast<char*>(external_array[k].ptr);
            }
            throw;
        }
    }

    explicit Deque(size_t count) : Deque(count, T()) {}

    explicit Deque(size_t count, const T& value)
        : external_size((count - 1) / BUCKET_SIZE + 1),
          external_array(new Node[external_size]),
          front_pos(0),
          back_pos(external_size - 1),
          _size(count) {
        size_t i = 0;
        try {
            for (; i < external_size; ++i) {
                size_t real_size_bucket =
                    (count > BUCKET_SIZE ? BUCKET_SIZE : count);
                delete[] external_array[i].ptr;
                external_array[i].ptr =
                    reinterpret_cast<T*>(new char[BUCKET_SIZE * sizeof(T)]);
                external_array[i].begin = 0;
                for (size_t j = 0; j < real_size_bucket; ++j) {
                    new (external_array[i].ptr + j) T(value);
                    external_array[i].end = j;
                }
                count -= real_size_bucket;
            }
        } catch (...) {
            for (size_t k = 0; k <= i; ++k) {
                for (size_t l = external_array[k].begin;
                     l <= external_array[k].end; ++l) {
                    (external_array[k].ptr + l)->~T();
                }
                delete[] reinterpret_cast<char*>(external_array[k].ptr);
            }
            delete[] external_array;
            throw;
        }
    }

    void push_back(const T& value) {
        if (_size > 0 && external_array[back_pos].end + 1 < BUCKET_SIZE) {
            new (external_array[back_pos].ptr + external_array[back_pos].end +
                 1) T(value);
            ++external_array[back_pos].end;
        } else {
            if (_size == 0) {
                back_pos = external_size / 2;
                front_pos = external_size / 2;
            } else {
                ++back_pos;
            }
            if (back_pos >= external_size) {
                increase_external_array();
            }
            try {
                if (external_array[back_pos].ptr == nullptr) {
                    external_array[back_pos].ptr =
                        reinterpret_cast<T*>(new char[BUCKET_SIZE * sizeof(T)]);
                }
                new (external_array[back_pos].ptr) T(value);
                external_array[back_pos].begin = 0;
                external_array[back_pos].end = 0;
            } catch (...) {
                delete[] reinterpret_cast<char*>(external_array[back_pos].ptr);
                external_array[back_pos].ptr = nullptr;
                if (_size != 0) {
                    --back_pos;
                }
                throw;
            }
        }
        ++_size;
    }

    void pop_back() {
        (external_array[back_pos].ptr + external_array[back_pos].end)->~T();
        if (external_array[back_pos].end > 0) {
            --external_array[back_pos].end;
        } else {
            if (_size > 1) {
                --back_pos;
                external_array[back_pos].end = BUCKET_SIZE - 1;
            } else {
                external_array[back_pos].begin = external_array[back_pos].end =
                    NOTHING;
            }
        }
        --_size;
    }

    void pop_front() {
        (external_array[front_pos].ptr + external_array[front_pos].begin)->~T();
        if (external_array[front_pos].begin < BUCKET_SIZE - 1) {
            ++external_array[front_pos].begin;
        } else {
            if (_size > 1) {
                ++front_pos;
                external_array[front_pos].begin = 0;
            } else {
                external_array[back_pos].begin = external_array[back_pos].end =
                    NOTHING;
            }
        }
        --_size;
    }

    void push_front(const T& value) {
        if (_size > 0 && external_array[front_pos].begin >= 1) {
            new (external_array[front_pos].ptr +
                 (external_array[front_pos].begin - 1)) T(value);
            --external_array[front_pos].begin;
        } else {
            if (_size == 0) {
                back_pos = external_size / 2;
                front_pos = external_size / 2;
            }
            if (front_pos <= 0) {
                increase_external_array();
            }
            if (_size != 0) {
                --front_pos;
            }
            try {
                if (external_array[front_pos].ptr == nullptr) {
                    external_array[front_pos].ptr =
                        reinterpret_cast<T*>(new char[BUCKET_SIZE * sizeof(T)]);
                }
                new (external_array[front_pos].ptr + (BUCKET_SIZE - 1))
                    T(value);
                external_array[front_pos].begin = BUCKET_SIZE - 1;
                external_array[front_pos].end = BUCKET_SIZE - 1;
            } catch (...) {
                delete[] reinterpret_cast<char*>(external_array[front_pos].ptr);
                external_array[front_pos].ptr = nullptr;
                if (_size != 0) {
                    ++front_pos;
                }
                throw;
            }
        }
        ++_size;
    }

    size_t size() const {
        return _size;
    }

    T& operator[](size_t i) {
        auto it = begin();
        it += i;
        return *it;
    }

    const T& operator[](size_t i) const {
        auto it = begin();
        it += i;
        return *it;
    }

    T& at(size_t i) {
        if (i >= _size) {
            throw std::out_of_range("out_of_range");
        }
        return operator[](i);
    }

    const T& at(size_t i) const {
        if (i >= _size) {
            throw std::out_of_range("out_of_range");
        }
        return operator[](i);
    }

    void insert(iterator where, const T& value) {
        if (where == begin()) {
            push_front(value);
            return;
        }
        if (where == end()) {
            push_back(value);
            return;
        }
        push_back(value);
        for (iterator it = end() - 1; it != where; --it) {
            new (it.ptr_node->ptr + it.index) T(*(it - 1));
        }
        new (where.ptr_node->ptr + where.index) T(value);
    }

    void erase(iterator where) {
        if (where == begin()) {
            pop_front();
            return;
        }
        if (where == end() - 1) {
            pop_back();
            return;
        }
        for (iterator it = where; it != end() - 1; ++it) {
            new (it.ptr_node->ptr + it.index) T(*(it + 1));
        }
        pop_back();
    }

    ~Deque() {
        for (size_t i = 0; i < external_size; ++i) {
            if (external_array[i].ptr != nullptr) {
                for (size_t j = external_array[i].begin;
                     j <= external_array[i].end; ++j) {
                    (external_array[i].ptr + j)->~T();
                }
                delete[] reinterpret_cast<char*>(external_array[i].ptr);
            }
        }
        delete[] external_array;
    }
};
