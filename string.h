#include <cstring>
#include <iostream>

class String {
  private:
    size_t size_ = 0;
    size_t capacity_ = 0;
    char* array_ = nullptr;

    explicit String(int count);
    void swap(String& str);
    void change_capacity(size_t new_capacity);
    static bool check(char* str_begin, const char* str_end, char* substr_begin);
    size_t find(const String& substr, size_t start, size_t stop,
                int step) const;

  public:
    String(const char* str);
    String();
    String(size_t n, char ch);
    String(const String& str);
    String& operator=(String str);
    char& operator[](size_t index);
    const char& operator[](size_t index) const;
    size_t length() const;
    size_t size() const;
    size_t capacity() const;
    void push_back(char ch);
    void pop_back();
    const char& front() const;
    char& front();
    const char& back() const;
    char& back();
    String& operator+=(const String& other);
    String& operator+=(char ch);
    size_t find(const String& substr) const;
    size_t rfind(const String& substr) const;
    String substr(size_t start, int count) const;
    bool empty() const;
    void clear();
    void shrink_to_fit();
    char* data();
    const char* data() const;
    ~String();
};

bool operator<(const String& a, const String& b);

bool operator>(const String& a, const String& b);

bool operator<=(const String& a, const String& b);

bool operator>=(const String& a, const String& b);

bool operator==(const String& a, const String& b);

bool operator!=(const String& a, const String& b);

std::ostream& operator<<(std::ostream& out, const String& str);

std::istream& operator>>(std::istream& in, String& str);

String operator+(const String& a, const String& b);

String operator+(const String& a, char ch);

String operator+(char ch, const String& a);
