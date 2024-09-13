#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

enum class Sign { POSITIVE, NEGATIVE };

class BigInteger {
    friend bool operator<(const BigInteger& a, const BigInteger& b);
    friend bool operator==(const BigInteger& a, const BigInteger& b);

  private:
    static Sign signum(int64_t x);
    bool isPositive() const;
    void clearZeros();
    static void clearZeros(std::vector<int64_t>& v);
    void multiplyBase();
    static Sign getSignForMultiply(const BigInteger& a, const BigInteger& b);
    std::pair<BigInteger, BigInteger> division(const BigInteger& other);
    BigInteger& decreaseForPositive(const std::vector<int64_t>& my_digits,
                                    const std::vector<int64_t>& other_digits);
    BigInteger& increaseForPositive(const std::vector<int64_t>& other_digits);
    BigInteger& decrease(const std::vector<int64_t>& other_digits);

    static const int64_t base = 1e9;
    static const int len = 9;
    std::vector<int64_t> digits_;
    Sign sign_ = Sign::POSITIVE;

  public:
    BigInteger() = default;
    BigInteger(int64_t n);
    BigInteger(const std::string& str, size_t size);
    explicit BigInteger(const std::string& str);
    std::string toString() const;
    BigInteger& operator+=(const BigInteger& other);
    BigInteger& operator*=(const BigInteger& other);
    BigInteger& operator/=(const BigInteger& other);
    BigInteger& operator%=(const BigInteger& other);
    BigInteger& operator-=(const BigInteger& other);
    BigInteger operator-();
    BigInteger& operator++();
    BigInteger operator++(int);
    explicit operator bool() const;
    void invert();
    Sign getSign() const;
};

std::ostream& operator<<(std::ostream& out, const BigInteger& big_integer);

BigInteger operator""_bi(const char* str, size_t size);

BigInteger operator""_bi(unsigned long long n);

BigInteger operator+(BigInteger a, const BigInteger& b);

BigInteger operator-(BigInteger a, const BigInteger& b);

BigInteger operator*(BigInteger a, const BigInteger& b);

BigInteger operator/(BigInteger a, const BigInteger& b);

BigInteger operator%(BigInteger a, const BigInteger& b);

bool operator<(const BigInteger& a, const BigInteger& b);

bool comparePositive(const std::vector<int64_t>& a,
                     const std::vector<int64_t>& b, bool strictly = true);

bool operator>(const BigInteger& a, const BigInteger& b);

bool operator<=(const BigInteger& a, const BigInteger& b);

bool operator>=(const BigInteger& a, const BigInteger& b);

bool operator==(const BigInteger& a, const BigInteger& b);

bool operator!=(const BigInteger& a, const BigInteger& b);

std::istream& operator>>(std::istream& in, BigInteger& big_integer);

class Rational {
    friend bool operator<(const Rational& a, const Rational& b);
    friend bool operator==(const Rational& a, const Rational& b);

  private:
    static BigInteger gcd(BigInteger a, BigInteger b);
    static BigInteger pow(int a, size_t b);
    void normalize();

    BigInteger numerator_;
    BigInteger denominator_;

  public:
    Rational(const BigInteger& n);
    Rational(int n);
    Rational() = default;
    Rational operator-();
    Rational& operator+=(const Rational& other);
    Rational& operator*=(const Rational& other);
    Rational& operator/=(const Rational& other);
    Rational& operator-=(const Rational& other);
    std::string toString() const;
    Sign getSign() const;
    std::string asDecimal(size_t precision) const;
    explicit operator double() const;
};

Rational operator+(Rational a, const Rational& b);

Rational operator-(Rational a, const Rational& b);

Rational operator/(Rational a, const Rational& b);

Rational operator*(Rational a, const Rational& b);

bool operator<(const Rational& a, const Rational& b);

bool operator>(const Rational& a, const Rational& b);

bool operator<=(const Rational& a, const Rational& b);

bool operator>=(const Rational& a, const Rational& b);

bool operator==(const Rational& a, const Rational& b);

bool operator!=(const Rational& a, const Rational& b);
