#include <iostream>
#include <vector>
#include <string>

class BigInteger;

BigInteger operator+(const BigInteger& lhs, const BigInteger& rhs);
BigInteger operator-(const BigInteger& lhs, const BigInteger& rhs);
BigInteger operator*(const BigInteger& lhs, const BigInteger& rhs);
BigInteger operator/(const BigInteger& lhs, const BigInteger& rhs);
BigInteger operator%(const BigInteger& lhs, const BigInteger& rhs);

bool operator<(const BigInteger& lhs, const BigInteger& rhs);
bool operator>(const BigInteger& lhs, const BigInteger& rhs);
bool operator<=(const BigInteger& lhs, const BigInteger& rhs);
bool operator>=(const BigInteger& lhs, const BigInteger& rhs);
bool operator==(const BigInteger& lhs, const BigInteger& rhs);
bool operator!=(const BigInteger& lhs, const BigInteger& rhs);

class BigInteger {
public:
    BigInteger(): is_negative_(false) {}

    BigInteger(int value) {
        if (value < 0) {
            is_negative_ = true;
            value *= -1;
        }
        else {
            is_negative_ = false;

        }
        if (value == 0) {
            number_.push_back(0);
        }

        while (value != 0) {
            number_.push_back(value % digitSize_);
            value /= digitSize_;
        }
    }

    BigInteger(const std::string& number) {
        is_negative_ = number[0] == '-';
        int part_length = digitCounter_;
        size_t modulo = (number.size() - static_cast<int>(is_negative_)) % part_length;
        int start = static_cast<int>(is_negative_);
        int sign = static_cast<int>(is_negative_);
        std::string part;
        while (start - number.length() + modulo != 0) {
            part = number[number.length() - start - 1 + sign] + part;
            if (part.length() == digitCounter_) {
                number_.push_back(std::stoi(part));
                part.clear();
            }
            start++;
        }

        part.clear();
        for (size_t i = sign; i < modulo + sign; ++i) {
            part += number[i];
        }
        if (!part.empty()) {
            number_.push_back(std::stoi(part));
        }
    }

    BigInteger(const BigInteger& other): is_negative_(other.is_negative_), number_(other.number_) {}

    BigInteger operator=(const BigInteger& other) {
        is_negative_ = other.is_negative_;
        number_ = other.number_;
        return *this;
    }

    BigInteger& operator+=(const BigInteger& rhs) {
        if (IsNegative() == rhs.IsNegative()) {
            Plus(*this, rhs);
            return *this;
        }

        BigInteger lhs_abs = Abs();
        BigInteger rhs_abs = rhs.Abs();

        if (lhs_abs == rhs_abs) {
            *this = 0;
            return *this;
        }

        if (lhs_abs > rhs_abs) {
            Minus(*this, rhs);
            return *this;
        }

        BigInteger copy = rhs;
        Minus(copy, *this);
        number_ = copy.number_;
        is_negative_ = !is_negative_;
        return *this;
    }

    BigInteger& operator-=(const BigInteger& rhs) {
        if (IsNegative() != rhs.IsNegative()) {
            Plus(*this, rhs);
            return *this;
        }

        BigInteger lhs_abs = Abs();
        BigInteger rhs_abs = rhs.Abs();

        if (lhs_abs == rhs_abs) {
            *this = 0;
            return *this;
        }

        if (lhs_abs > rhs_abs) {
            Minus(*this, rhs);
            return *this;
        }

        BigInteger copy = rhs;
        Minus(copy, *this);
        number_ = copy.number_;
        is_negative_ = !is_negative_;
        return *this;
    }

    BigInteger& operator*=(const BigInteger& rhs) {
        Multiply(*this, rhs);
        is_negative_ = is_negative_ != rhs.is_negative_;
        return *this;
    }

    BigInteger& operator/=(const BigInteger& rhs) {
        if (Abs() < rhs.Abs()) {
            *this = 0;
            return *this;
        }

        Divide(*this, rhs);
        is_negative_ = is_negative_ != rhs.is_negative_;
        return *this;
    }

    BigInteger& operator%=(const BigInteger& rhs) {
        BigInteger copy = rhs.Abs();
        *this -= copy * (*this / copy);
        return *this;
    }

    BigInteger operator-() {
        if (*this == 0) {
            return 0;
        }

        BigInteger copy = *this;
        copy.is_negative_ = !is_negative_;
        return copy;
    }

    BigInteger operator+() {
        return *this;
    }

    BigInteger& operator++() {
        *this += 1;
        return *this;
    }

    BigInteger operator++(int) {
        BigInteger copy = *this;
        ++*this;
        return copy;
    }

    BigInteger& operator--() {
        *this -= 1;
        return *this;
    }

    BigInteger operator--(int) {
        BigInteger copy = *this;
        --*this;
        return copy;
    }

    std::string toString() const {
        std::string str = (is_negative_ ? "-" : "");
        str += std::to_string(number_[Length() - 1]);
        std::string part;
        for (size_t i = 0; i < Length() - 1; ++i) {
            part = std::to_string(number_[Length() - i - 2]);
            part = std::string(digitCounter_ - part.length(), '0') + part;
            str += part;
        }
        return str;
    }

    explicit operator int() const {
        std::string number = toString();
        return std::stoi(number);
    }

    explicit operator bool() const {
        return !(*this == 0);
    }

    long long operator[](int position) const {
        return number_[position];
    }

    long long& operator[](int position) {
        return number_[position];
    }

    bool IsNegative() const {
        return is_negative_;
    }

    size_t Length() const {
        return number_.size();
    }

    BigInteger Abs() const {
        BigInteger copy = *this;
        copy.is_negative_ = false;
        return copy;
    }

    bool isEven() const {
        return !(number_[Length() - 1] % 2);
    }

private:
    void Trim() {
        size_t size = Length();
        while (size > 1 && number_[size - 1] == 0) {
            number_.pop_back();
            size--;
        }
    }

    void Plus(BigInteger& first, const BigInteger& second) {
        size_t boundary = std::max(first.Length(), second.Length());
        for (size_t i = 0; i < boundary; ++i) {
            if (i == first.Length()) {
                first.number_.push_back(0);
            }
            if (i < second.Length()) {
                first[i] += second[i];
            }
        }

        size_t size = first.Length();
        for (size_t i = 0; i < size; ++i) {
            if (first[i] >= digitSize_) {
                first[i] -= digitSize_;
                if (i + 1 == first.Length()) {
                    first.number_.push_back(0);
                }
                first[i + 1]++;
            }
        }

        first.Trim();
    }

    void Minus(BigInteger& first, const BigInteger& second) {
        for (size_t i = 0; i < second.Length(); ++i) {
            first[i] -= second[i];
        }

        for (size_t i = 0; i < first.Length(); ++i) {
            if (first[i] < 0) {
                first[i] += digitSize_;
                first[i + 1]--;
            }
        }

        first.Trim();
    }

    void Multiply(BigInteger& first, const BigInteger& second) {
        BigInteger product;
        product.number_.resize(first.Length() + second.Length() + 10);
        for (size_t i = 0; i < first.Length(); ++i) {
            for (size_t  j = 0; j < second.Length(); ++j) {
                product[i + j] += (first[i] * second[j]) % digitSize_;
                product[i + j + 1] += (first[i] * second[j]) / digitSize_;
            }
        }

        for (size_t i = 0; i < product.Length() - 1; ++i) {
            product[i + 1] += product[i] / digitSize_;
            product[i] %= digitSize_;
        }

        product.Trim();
        first.number_ = product.number_;
    }

    void Divide(BigInteger& first, const BigInteger& second) {
        BigInteger copySecond = second.Abs();
        BigInteger copyFirst = first.Abs();
        BigInteger divisor;
        BigInteger quotient;
        quotient.number_.resize(copyFirst.Length() - second.Length() + 1);

        while (copySecond <= copyFirst) {
            size_t shift = copyFirst.Length() - second.Length();
            divisor = ShiftLeft(copySecond, shift);
            if (divisor > copyFirst) {
                divisor = ShiftRight(divisor, 1);
                shift--;
            }
            long long quoDigit = Find(copyFirst, divisor);
            Multiply(divisor, quoDigit);
            Minus(copyFirst, divisor);
            quotient[shift] = quoDigit;
            divisor = copySecond;
        }

        quotient.Trim();
        first.number_ = quotient.number_;
    }

    int Find(const BigInteger& first, const BigInteger& second) {
        int left = 0, right = digitSize_;
        while (right - left > 1) {
            int middle = (left + right) >> 1;
            if (middle * second.Abs() <= first.Abs()) {
                left = middle;
            }
            else {
                right = middle;
            }
        }
        return left;
    }

    BigInteger ShiftRight(const BigInteger& number, size_t shift) {
        if (number.Length() <= shift) {
            return 0;
        }

        BigInteger copy;
        copy.number_.resize(number.Length() - shift);
        for (size_t i = number.Length() - 1; i >= shift; --i) {
            copy[i - shift] = number[i];
        }
        return copy;
    }

    BigInteger ShiftLeft(const BigInteger& number, size_t shift) {
        BigInteger copy;
        copy.number_.resize(number.Length() + shift);
        for (size_t i = 0; i < number.Length(); ++i) {
            copy[i + shift] = number[i];
        }
        return copy;
    }

    static const int digitSize_ = 1000000000;
    static const int digitCounter_ = 9;
    bool is_negative_;
    std::vector<long long> number_;
};

BigInteger operator+(const BigInteger& lhs, const BigInteger& rhs) {
    BigInteger copy = lhs;
    copy += rhs;
    return copy;
}

BigInteger operator-(const BigInteger& lhs, const BigInteger& rhs) {
    BigInteger copy = lhs;
    copy -= rhs;
    return copy;
}

BigInteger operator*(const BigInteger& lhs, const BigInteger& rhs) {
    BigInteger copy = lhs;
    copy *= rhs;
    return copy;
}

BigInteger operator/(const BigInteger& lhs, const BigInteger& rhs) {
    BigInteger copy = lhs;
    copy /= rhs;
    return copy;
}

BigInteger operator%(const BigInteger& lhs, const BigInteger& rhs) {
    BigInteger copy = lhs;
    copy %= rhs;
    return copy;
}

bool operator<(const BigInteger& lhs, const BigInteger& rhs) {
    if (lhs.IsNegative() != rhs.IsNegative()) {
        return lhs.IsNegative();
    }

    if (lhs.Length() < rhs.Length()) {
        return !lhs.IsNegative();
    }

    if (lhs.Length() > rhs.Length()) {
        return lhs.IsNegative();
    }

    for (size_t i = 0; i < lhs.Length(); ++i) {
        if (lhs[lhs.Length() - 1 - i] > rhs[lhs.Length() - 1 - i]) {
            return lhs.IsNegative();
        }
        if (lhs[lhs.Length() - 1 - i] < rhs[lhs.Length() - 1 - i]) {
            return !lhs.IsNegative();
        }
    }
    return false;
}

bool operator>(const BigInteger& lhs, const BigInteger& rhs) {
    return rhs < lhs;
}

bool operator>=(const BigInteger& lhs, const BigInteger& rhs) {
    return !(lhs < rhs);
}

bool operator<=(const BigInteger& lhs, const BigInteger& rhs) {
    return !(lhs > rhs);
}

bool operator==(const BigInteger& lhs, const BigInteger& rhs) {
    if (lhs.Length() != rhs.Length()) {
        return false;
    }

    if (lhs.IsNegative() != rhs.IsNegative()) {
        return false;
    }

    for (size_t i = 0; i < lhs.Length(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(const BigInteger& lhs, const BigInteger& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& out, const BigInteger& number) {
    std::string value = number.toString();
    out << value;
    return out;
}

std::istream& operator>>(std::istream& in, BigInteger& number) {
    std::string value;
    in >> value;
    BigInteger copy(value);
    number = copy;
    return in;
}

class Rational;

Rational operator+(const Rational& lhs, const Rational& rhs);
Rational operator-(const Rational& lhs, const Rational& rhs);
Rational operator*(const Rational& lhs, const Rational& rhs);
Rational operator/(const Rational& lhs, const Rational& rhs);

bool operator<(const Rational& lhs, const Rational& rhs);
bool operator>(const Rational& lhs, const Rational& rhs);
bool operator<=(const Rational& lhs, const Rational& rhs);
bool operator>=(const Rational& lhs, const Rational& rhs);
bool operator==(const Rational& lhs, const Rational& rhs);
bool operator!=(const Rational& lhs, const Rational& rhs);

class Rational {
public:
    Rational() = default;

    Rational(const BigInteger& value): numerator_(value), denominator_(1) {}

    Rational(int value): numerator_(value), denominator_(1) {}

    Rational(const BigInteger& value_numerator, const BigInteger& value_denominator): numerator_(value_numerator), denominator_(value_denominator) {
        Reform();
    }

    Rational(const Rational& other): numerator_(other.numerator_), denominator_(other.denominator_) {}

    Rational& operator=(const Rational& other) {
        numerator_ = other.numerator_;
        denominator_ = other.denominator_;
        return *this;
    }

    Rational& operator+=(const Rational& other) {
        numerator_ = numerator_ * other.denominator_ + other.numerator_ * denominator_;
        denominator_ *= other.denominator_;
        Reform();
        return *this;
    }

    Rational& operator-=(const Rational& other) {
        numerator_ = numerator_ * other.denominator_ - other.numerator_ * denominator_;
        denominator_ *= other.denominator_;
        Reform();
        return *this;
    }

    Rational& operator*=(const Rational& other) {
        numerator_ *= other.numerator_;
        denominator_ *= other.denominator_;
        Reform();
        return *this;
    }

    Rational& operator/=(const Rational& other) {
        numerator_ *= other.denominator_;
        denominator_ *= other.numerator_;
        Reform();
        return *this;
    }

    Rational operator-() {
        if (numerator_ == 0) {
            return 0;
        }

        Rational copy = *this;
        copy.numerator_ *= -1;
        return copy;
    }

    std::string toString() const {
        std::string str = numerator_.toString();
        if (denominator_ != 1) {
            str += "/" + denominator_.Abs().toString();
        }
        return str;
    }

    std::string asDecimal(size_t precision = 0) {
        std::string str = (numerator_.IsNegative() ? "-" : "");
        Rational copy(numerator_.Abs(), denominator_.Abs());
        str += (copy.numerator_ / copy.denominator_).toString();
        if (precision == 0) {
            return str;
        }

        str += ".";
        copy.numerator_ %= copy.denominator_;
        static const int system_size = 10;
        for (size_t i = 0; i < precision; ++i) {
            copy.numerator_ *= system_size;;
            str += (copy.numerator_ / copy.denominator_).toString();
            copy.numerator_ %= copy.denominator_;
        }

        int size = str.length() - 1;
        static const int zero_order = 48;
        while (str[size] == zero_order) {
            --size;
        }
        if (str[size] < zero_order) {
            return "0";
        }
        return str.substr(0, size + 1);
    }

    explicit operator double() {
        static const int counter = 100;
        return std::stod(asDecimal(counter));
    }

    bool IsNegative() const {
        return numerator_.IsNegative();
    }

private:
    BigInteger Max(const BigInteger& first, const BigInteger& second) {
        if (first > second) {
            return first;
        }
        return second;
    }

    BigInteger Min(const BigInteger& first, const BigInteger& second) {
        if (first < second) {
            return first;
        }
        return second;
    }

    BigInteger Gcd(const BigInteger& numerator, const BigInteger& denominator) {
        BigInteger copy_max = Max(numerator.Abs(), denominator.Abs());
        BigInteger copy_min = Min(numerator.Abs(), denominator.Abs());
        while (copy_min != 0) {
            BigInteger help = copy_min;
            copy_min = copy_max % copy_min;
            copy_max = help;
        }
        return copy_max;
    }

    void Reform() {
        BigInteger value = Gcd(numerator_, denominator_);
        numerator_ /= value;
        denominator_ /= value;

        if (denominator_.IsNegative()) {
            numerator_ *= -1;
            denominator_ *= -1;
        }

        if (numerator_ == 0 && numerator_.IsNegative()) {
            numerator_ *= -1;
        }
    }

    BigInteger numerator_;
    BigInteger denominator_;

};

Rational operator+(const Rational& lhs, const Rational& rhs) {
    Rational copy = lhs;
    copy += rhs;
    return copy;
}

Rational operator-(const Rational& lhs, const Rational& rhs) {
    Rational copy = lhs;
    copy -= rhs;
    return copy;
}

Rational operator*(const Rational& lhs, const Rational& rhs) {
    Rational copy = lhs;
    copy *= rhs;
    return copy;
}

Rational operator/(const Rational& lhs, const Rational& rhs) {
    Rational copy = lhs;
    copy /= rhs;
    return copy;
}


bool operator<(const Rational& lhs, const Rational& rhs) {
    Rational value = lhs - rhs;
    if (value.IsNegative()) {
        return true;
    }
    return false;
}

bool operator>(const Rational& lhs, const Rational& rhs) {
    return rhs < lhs;
}

bool operator>=(const Rational& lhs, const Rational& rhs) {
    return !(lhs < rhs);
}

bool operator<=(const Rational& lhs, const Rational& rhs) {
    return !(lhs > rhs);
}

bool operator==(const Rational& lhs, const Rational& rhs) {
    return !(lhs-rhs).IsNegative() && !(rhs - lhs).IsNegative();
}

bool operator!=(const Rational& lhs, const Rational& rhs) {
    return !(lhs == rhs);
}
