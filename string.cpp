#include "string.h"
#include <cstring>
#include <iostream>
using std::cout, std::cin;

String::String(int count)
    : size_(count), capacity_(count), array_(new char[count + 1]) {
    array_[size_] = '\0';
}

void String::swap(String& str) {
    std::swap(array_, str.array_);
    std::swap(size_, str.size_);
    std::swap(capacity_, str.capacity_);
}

void String::change_capacity(size_t new_capacity) {
    capacity_ = std::max(1, static_cast<int>(new_capacity));
    char* new_arrays = new char[capacity_ + 1];
    memcpy(new_arrays, array_, size_ + 1);
    delete[] array_;
    array_ = new_arrays;
}

bool String::check(char* str_begin, const char* str_end, char* substr_begin) {
    while (str_begin != str_end) {
        if (*str_begin != *substr_begin) {
            return false;
        }
        ++str_begin;
        ++substr_begin;
    }
    return true;
}

size_t String::find(const String& substr, size_t start, size_t stop,
                    int step) const {
    for (size_t i = start; i != stop; i += step) {
        if (check(array_ + i, array_ + i + substr.size_, substr.array_)) {
            return i;
        }
    }
    return length();
}

String::String(const char* str)
    : size_(strlen(str)), capacity_(size_), array_(new char[capacity_ + 1]) {
    memcpy(array_, str, size_ + 1);
}

String::String() : String(0){};

String::String(size_t n, char ch)
    : size_(n), capacity_(n), array_(new char[n + 1]) {
    memset(array_, ch, n);
    array_[n] = '\0';
}

String::String(const String& str)
    : size_(str.size_), capacity_(str.size_), array_(new char[str.size_ + 1]) {
    memcpy(array_, str.array_, size_ + 1);
}

String& String::operator=(String str) {
    swap(str);
    return *this;
}

char& String::operator[](size_t index) {
    return array_[index];
}

const char& String::operator[](size_t index) const {
    return array_[index];
}

size_t String::length() const {
    return size_;
}

size_t String::size() const {
    return size_;
}

size_t String::capacity() const {
    return capacity_;
}

void String::push_back(char ch) {
    if (size_ == capacity_) {
        change_capacity(capacity_ * 2);
    }
    array_[size_] = ch;
    ++size_;
    array_[size_] = '\0';
}

void String::pop_back() {
    --size_;
    array_[size_] = '\0';
}

const char& String::front() const {
    return array_[0];
}

char& String::front() {
    return array_[0];
}

const char& String::back() const {
    return array_[size_ - 1];
}

char& String::back() {
    return array_[size_ - 1];
}

String& String::operator+=(const String& other) {
    if (size_ + other.size_ > capacity_) {
        change_capacity(size_ + other.size_);
    }
    memcpy(array_ + size_, other.array_, other.size_);
    size_ += other.size_;
    array_[size_] = '\0';
    return *this;
}

String& String::operator+=(const char ch) {
    push_back(ch);
    return *this;
}

size_t String::find(const String& substr) const {
    return find(substr, 0, size_ - substr.size_ + 1, 1);
}

size_t String::rfind(const String& substr) const {
    return find(substr, size_ - substr.size_, substr.size_ - 2, -1);
}

String String::substr(size_t start, int count) const {
    String result(count);
    memcpy(result.array_, array_ + start, count);
    return result;
}

bool String::empty() const {
    return size_ == 0;
}

void String::clear() {
    size_ = 0;
    array_[size_] = '\0';
}

void String::shrink_to_fit() {
    change_capacity(size_);
}

char* String::data() {
    return array_;
}

const char* String::data() const {
    return array_;
}

String::~String() {
    delete[] array_;
}

bool operator<(const String& a, const String& b) {
    for (size_t i = 0; i < a.size() && i < b.size(); ++i) {
        if (a[i] != b[i]) {
            return a[i] < b[i];
        }
    }
    return a.size() < b.size();
}

bool operator>(const String& a, const String& b) {
    return b < a;
}

bool operator<=(const String& a, const String& b) {
    return !(a > b);
}

bool operator>=(const String& a, const String& b) {
    return !(a < b);
}

bool operator==(const String& a, const String& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(const String& a, const String& b) {
    return !(a == b);
}

std::ostream& operator<<(std::ostream& out, const String& str) {
    out << str.data();
    return out;
}

std::istream& operator>>(std::istream& in, String& str) {
    str.clear();
    char tmp;
    while (in.get(tmp)) {
        if (std::isspace(tmp) != 0) {
            break;
        }
        str.push_back(tmp);
    }
    return in;
}

String operator+(const String& a, const String& b) {
    String result = a;
    result += b;
    return result;
}

String operator+(const String& a, char ch) {
    String result = a;
    result += ch;
    return result;
}

String operator+(char ch, const String& a) {
    String result(1, ch);
    result += a;
    return result;
}
