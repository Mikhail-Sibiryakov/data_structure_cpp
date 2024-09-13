#pragma once
#include <cstddef>
#include <iostream>
#include <memory>

template <int N>
class StackStorage {
  public:
    StackStorage()
        : _size(N), _ptr(_storage) {}
    StackStorage(const StackStorage<N>& other) = delete;

    template <typename T>
    T* reserve(size_t count) {
        size_t bytes = sizeof(T) * count;
        if (std::align(alignof(T), bytes, _ptr, _size)) {
            T* res = reinterpret_cast<T*>(_ptr);
            _ptr = reinterpret_cast<char*>(_ptr) + bytes;
            _size -= bytes;
            return res;
        }
        return nullptr;
    }

  private:
    char _storage[N]{};
    size_t _size;
    void* _ptr;
};

template <typename T, int N>
class StackAllocator {
  public:
    using value_type = T;

    StackAllocator()
        : _pool(nullptr) {}
    StackAllocator(StackStorage<N>& tmp_pool)
        : _pool(&tmp_pool) {}
    template <typename U>
    StackAllocator(const StackAllocator<U, N>& other) {
        _pool = other.getPool();
    };
    StackAllocator& operator=(const StackAllocator<T, N>& other) = default;
    ~StackAllocator() = default;

    bool operator==(const StackAllocator<T, N>& other) {
        return _pool == other.getPool();
    }
    bool operator!=(const StackAllocator<T, N>& other) {
        return !(operator==(other));
    }

    T* allocate(size_t count) {
        return _pool->template reserve<T>(count);
    }

    void deallocate(T* ptr, size_t count) {
        std::ignore = ptr;
        std::ignore = count;
    }

    StackStorage<N>* getPool() const {
        return _pool;
    }

    template <typename U>
    struct rebind {
        using other = StackAllocator<U, N>;
    };

  private:
    StackStorage<N>* _pool;
    size_t _alignment{};
    size_t _size_of{};
};
