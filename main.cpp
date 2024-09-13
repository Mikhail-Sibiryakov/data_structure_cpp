#include <iostream>
#include "string.cpp"
#include "biginteger.cpp"
#include "deque.h"
#include "list.h"
#include "stack_allocator.h"

void SampleString() {
    String s(10, 's');
    cout << s + String("abacaba") << '\n';
}

void SampleBigInt() {
    BigInteger a = 2024;
    for (int i = 0; i < 100; ++i) {
        a *= 2024;
    }
    cout << a << '\n';
}
void SampleDeque() {
    Deque<int> d;
    d.push_back(1);
    d.push_back(2);
    d.push_front(1001);
    d.push_back(3);
    d.push_front(1003);
    for (auto el : d) {
        cout << el << ' ';
    }
    cout << '\n';
}

void SampleList() {
    StackStorage<200'000> storage;
    StackAllocator<char, 200'000> charalloc(storage);
    StackAllocator<int, 200'000> intalloc(charalloc);
    List<int, StackAllocator<int, 200'000>> l(intalloc);
    l.push_back(213);
    l.push_back(100);
    l.push_front(123912);
    for (auto el : l) {
        cout << el << ' ';
    }
    cout << '\n';
}

int main() {
    SampleString();
    SampleBigInt();
    SampleDeque();
    SampleList();
}
