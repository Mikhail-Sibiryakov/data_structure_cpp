#include "biginteger.h"

BigInteger::BigInteger(int64_t n) : sign_(signum(n)) {
    n = std::abs(n);
    if (n == 0) {
        digits_.push_back(0);
    }
    while (n > 0) {
        digits_.push_back(n % base);
        n /= base;
    }
}

std::ostream& operator<<(std::ostream& out, const BigInteger& bi) {
    out << bi.toString();
    return out;
}

BigInteger operator""_bi(const char* str, size_t size) {
    BigInteger result(str, size);
    return result;
}

BigInteger operator""_bi(unsigned long long n) {
    BigInteger result(n);
    return result;
}

BigInteger& BigInteger::decrease(const std::vector<int64_t>& other_digits) {
    if (comparePositive(digits_, other_digits)) {
        invert();
        return decreaseForPositive(other_digits, digits_);
    }
    return decreaseForPositive(digits_, other_digits);
}

BigInteger& BigInteger::operator+=(const BigInteger& other) {
    if (isPositive() == other.isPositive()) {
        return increaseForPositive(other.digits_);
    }
    return decrease(other.digits_);
}

BigInteger& BigInteger::operator-=(const BigInteger& other) {
    if (isPositive() == other.isPositive()) {
        return decrease(other.digits_);
    }
    return increaseForPositive(other.digits_);
}

Sign BigInteger::signum(int64_t x) {
    if (x < 0) {
        return Sign::NEGATIVE;
    }
    return Sign::POSITIVE;
}

BigInteger operator+(BigInteger a, const BigInteger& b) {
    a += b;
    return a;
}

BigInteger operator-(BigInteger a, const BigInteger& b) {
    a -= b;
    return a;
}

bool operator<(const BigInteger& a, const BigInteger& b) {
    if (a.sign_ != b.sign_) {
        return a.sign_ == Sign::NEGATIVE;
    }
    return (a.sign_ == Sign::NEGATIVE
                ? !comparePositive(a.digits_, b.digits_, false)
                : comparePositive(a.digits_, b.digits_));
}

bool comparePositive(const std::vector<int64_t>& a,
                     const std::vector<int64_t>& b, bool strictly) {
    if (a.size() != b.size()) {
        return a.size() < b.size();
    }
    for (size_t i = a.size(); i > 0; --i) {
        if (a[i - 1] != b[i - 1]) {
            return a[i - 1] < b[i - 1];
        }
    }
    return !strictly;
}

bool operator>(const BigInteger& a, const BigInteger& b) {
    return b < a;
}

bool operator<=(const BigInteger& a, const BigInteger& b) {
    return !(a > b);
}

bool operator>=(const BigInteger& a, const BigInteger& b) {
    return !(a < b);
}

bool operator==(const BigInteger& a, const BigInteger& b) {
    if (a.sign_ != b.sign_) {
        return false;
    }
    if (a.digits_.size() != b.digits_.size()) {
        return false;
    }
    for (size_t i = a.digits_.size(); i > 0; --i) {
        if (a.digits_[i - 1] != b.digits_[i - 1]) {
            return false;
        }
    }
    return true;
}

BigInteger& BigInteger::decreaseForPositive(
    const std::vector<int64_t>& my_digits,
    const std::vector<int64_t>& other_digits) {
    int carry = 0;
    for (size_t i = 0; i < my_digits.size() || carry != 0; ++i) {
        if (digits_.size() <= i) {
            digits_.resize(i + 1, 0);
        }
        digits_[i] = my_digits[i] - carry -
                     (i < other_digits.size() ? other_digits[i] : 0);
        if (digits_[i] < 0) {
            digits_[i] += base;
            carry = 1;
        } else {
            carry = 0;
        }
    }
    clearZeros(digits_);
    bool all_zero = true;
    for (auto i : digits_) {
        if (i != 0) {
            all_zero = false;
            break;
        }
    }
    sign_ = (all_zero ? Sign::POSITIVE : sign_);
    return *this;
}

BigInteger& BigInteger::increaseForPositive(
    const std::vector<int64_t>& other_digits) {
    int carry = 0;
    for (size_t i = 0; i < other_digits.size() || carry != 0; ++i) {
        if (i == digits_.size()) {
            digits_.push_back(0);
        }
        digits_[i] += other_digits[i] + carry;
        if (digits_[i] >= base) {
            digits_[i] -= base;
            carry = 1;
        } else {
            carry = 0;
        }
    }
    return *this;
}

BigInteger BigInteger::operator-() {
    if (*this == 0) {
        return *this;
    }
    BigInteger result = *this;
    result.sign_ = (sign_ == Sign::NEGATIVE ? Sign::POSITIVE : Sign::NEGATIVE);
    return result;
}

bool BigInteger::isPositive() const {
    return sign_ == Sign::POSITIVE;
}

BigInteger& BigInteger::operator++() {
    *this += 1;
    return *this;
}

BigInteger BigInteger::operator++(int) {
    BigInteger copy = *this;
    ++*this;
    return copy;
}

bool operator!=(const BigInteger& a, const BigInteger& b) {
    return !(a == b);
}

BigInteger operator*(BigInteger a, const BigInteger& b) {
    a *= b;
    return a;
}

void BigInteger::clearZeros() {
    clearZeros(digits_);
}

void BigInteger::clearZeros(std::vector<int64_t>& v) {
    while (v.size() > 1 && v.back() == 0) {
        v.pop_back();
    }
}

void BigInteger::multiplyBase() {
    digits_.push_back(0);
    for (size_t i = digits_.size(); i > 1; --i) {
        digits_[i - 1] = digits_[i - 2];
    }
}

BigInteger operator/(BigInteger a, const BigInteger& b) {
    a /= b;
    return a;
}

BigInteger operator%(BigInteger a, const BigInteger& b) {
    a %= b;
    return a;
}

Sign BigInteger::getSignForMultiply(const BigInteger& a, const BigInteger& b) {
    Sign sign = (a.sign_ != b.sign_ ? Sign::NEGATIVE : Sign::POSITIVE);
    if (a == 0 || b == 0) {
        sign = Sign::POSITIVE;
    }
    return sign;
}

BigInteger::BigInteger(const std::string& str, size_t size) {
    std::string tmp_str;
    size_t stop = 0;
    if (str[0] == '-') {
        sign_ = Sign::NEGATIVE;
        stop = 1;
    }
    for (size_t i = size; i > stop; --i) {
        tmp_str += str[i - 1];
        if (tmp_str.size() >= BigInteger::len || i == stop + 1) {
            std::reverse(tmp_str.begin(), tmp_str.end());
            digits_.push_back(std::stoi(tmp_str));
            tmp_str.clear();
        }
    }
}

std::string BigInteger::toString() const {
    std::string result;
    if (sign_ == Sign::NEGATIVE) {
        result += '-';
    }
    result += std::to_string(digits_.back());
    for (size_t i = digits_.size(); i > 1; --i) {
        std::string tmp = std::to_string(digits_[i - 2]);
        result += std::string(len - tmp.length(), '0');
        result += tmp;
    }
    return result;
}

BigInteger::operator bool() const {
    return *this != 0;
}

BigInteger& BigInteger::operator*=(const BigInteger& other) {
    BigInteger result;
    result.sign_ = BigInteger::getSignForMultiply(*this, other);
    result.digits_.resize(digits_.size() + other.digits_.size(), 0);
    for (size_t i = 0; i < digits_.size(); ++i) {
        int64_t carry = 0;
        for (size_t j = 0; j < other.digits_.size() || carry != 0; ++j) {
            int64_t cur =
                result.digits_[i + j] +
                digits_[i] * (j < other.digits_.size() ? other.digits_[j] : 0) +
                carry;
            result.digits_[i + j] = cur % BigInteger::base;
            carry = cur / BigInteger::base;
        }
    }
    result.clearZeros();
    *this = result;
    return *this;
}

BigInteger& BigInteger::operator/=(const BigInteger& other) {
    *this = division(other).first;
    return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger& other) {
    *this = division(other).second;
    return *this;
}

void BigInteger::invert() {
    sign_ = (sign_ == Sign::POSITIVE ? Sign::NEGATIVE : Sign::POSITIVE);
}

Sign BigInteger::getSign() const {
    return sign_;
}

BigInteger::BigInteger(const std::string& str)
    : BigInteger(str, str.length()) {}

std::pair<BigInteger, BigInteger> BigInteger::division(
    const BigInteger& other) {
    BigInteger a_copy = *this;
    BigInteger b_copy = other;
    assert(b_copy != 0);

    BigInteger result;
    Sign sign_for_remains = a_copy.getSign();
    Sign sign_for_result = BigInteger::getSignForMultiply(a_copy, b_copy);
    a_copy.sign_ = Sign::POSITIVE;
    b_copy.sign_ = Sign::POSITIVE;

    result.digits_.resize(a_copy.digits_.size(), 0);
    BigInteger cur_value;
    for (size_t i = a_copy.digits_.size(); i > 0; --i) {
        cur_value.multiplyBase();
        cur_value.digits_[0] = a_copy.digits_[i - 1];
        cur_value.clearZeros();
        int64_t x = 0;
        int64_t left = 0;
        int64_t right = BigInteger::base;
        while (left <= right) {
            int64_t mid = (left + right) >> 1;
            BigInteger cur = b_copy * mid;
            if (cur <= cur_value) {
                x = mid;
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        result.digits_[i - 1] = x;
        cur_value -= b_copy * x;
    }
    result.clearZeros();
    if (result == 0) {
        result.sign_ = Sign::POSITIVE;
    } else {
        result.sign_ = sign_for_result;
    }
    cur_value.sign_ = sign_for_remains;
    return {result, cur_value};
}

std::istream& operator>>(std::istream& in, BigInteger& big_integer) {
    std::string str;
    in >> str;
    big_integer = BigInteger(str, str.length());
    return in;
}

Rational::Rational(const BigInteger& n) : numerator_(n), denominator_(1){};

Rational Rational::operator-() {
    Rational result = *this;
    result.numerator_.invert();
    return result;
}

Rational& Rational::operator+=(const Rational& other) {
    numerator_ =
        (numerator_ * other.denominator_) + (denominator_ * other.numerator_);
    denominator_ *= other.denominator_;
    normalize();
    return *this;
}

BigInteger Rational::gcd(BigInteger a, BigInteger b) {
    if (a.getSign() == Sign::NEGATIVE) {
        a.invert();
    }
    if (b.getSign() == Sign::NEGATIVE) {
        b.invert();
    }
    while (b > 0) {
        BigInteger tmp = a;
        a = b;
        b = tmp % b;
    }
    return a;
}

std::string Rational::toString() const {
    if (numerator_ == 0) {
        return "0";
    }
    std::string result = numerator_.toString();
    if (denominator_ != 1) {
        result += '/';
        result += denominator_.toString();
    }
    return result;
}

Rational::Rational(int n) : Rational(BigInteger(n)) {}

Rational& Rational::operator-=(const Rational& other) {
    numerator_ =
        (numerator_ * other.denominator_) - (denominator_ * other.numerator_);
    denominator_ *= other.denominator_;
    normalize();
    return *this;
}

void Rational::normalize() {
    if (denominator_.getSign() == Sign::NEGATIVE) {
        denominator_.invert();
        numerator_.invert();
    }
    BigInteger divider = gcd(numerator_, denominator_);
    numerator_ /= divider;
    denominator_ /= divider;
}

Rational& Rational::operator*=(const Rational& other) {
    numerator_ *= other.numerator_;
    denominator_ *= other.denominator_;
    normalize();
    return *this;
}

Rational& Rational::operator/=(const Rational& other) {
    numerator_ *= other.denominator_;
    denominator_ *= other.numerator_;
    normalize();
    return *this;
}

Rational operator+(Rational a, const Rational& b) {
    a += b;
    return a;
}

Rational operator-(Rational a, const Rational& b) {
    a -= b;
    return a;
}

Rational operator/(Rational a, const Rational& b) {
    a /= b;
    return a;
}

Rational operator*(Rational a, const Rational& b) {
    a *= b;
    return a;
}

bool operator<(const Rational& a, const Rational& b) {
    if (a.getSign() != b.getSign()) {
        return a.getSign() == Sign::NEGATIVE;
    }
    return a.numerator_ * b.denominator_ < b.numerator_ * a.denominator_;
}

bool operator>(const Rational& a, const Rational& b) {
    return b < a;
}

bool operator<=(const Rational& a, const Rational& b) {
    return !(a > b);
}

bool operator>=(const Rational& a, const Rational& b) {
    return !(a < b);
}

bool operator==(const Rational& a, const Rational& b) {
    if (a.getSign() != b.getSign()) {
        return false;
    }
    return (a.numerator_ == b.numerator_) && (a.denominator_ == b.denominator_);
}

bool operator!=(const Rational& a, const Rational& b) {
    return !(a == b);
}
Sign Rational::getSign() const {
    return numerator_.getSign();
}

std::string Rational::asDecimal(size_t precision) const {
    std::string result;
    BigInteger div = numerator_ / denominator_;
    BigInteger mod = numerator_ - (div * denominator_);
    mod *= pow(10, precision);
    result = div.toString();
    result += '.';
    BigInteger fractional_part = mod / denominator_;
    if (fractional_part.getSign() == Sign::NEGATIVE) {
        fractional_part.invert();
    }
    std::string fractional_part_str = fractional_part.toString();
    if (fractional_part_str.length() < precision) {
        result += std::string(precision - fractional_part_str.length(), '0');
    }
    result += fractional_part_str;
    if (getSign() == Sign::NEGATIVE && result[0] != '-') {
        result = "-" + result;
    }
    return result;
}

Rational::operator double() const {
    return std::stof((asDecimal(20)));
}

BigInteger Rational::pow(int a, size_t b) {
    BigInteger ans = 1;
    for (size_t i = 0; i < b; ++i) {
        ans *= a;
    }
    return ans;
}
