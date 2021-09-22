#include "number.h"

// constructors
number_t::number_t() = default;
number_t::number_t(long value) : _value{value} {}

void number_t::swap(number_t &other) {
  std::swap(_value, other._value);
}

// assignment operator
number_t &number_t::operator=(const number_t &other) {
  number_t tmp(other);
  swap(tmp);
  return *this;
}

// casts
number_t::operator bool() const {
  return _value != 0;
}

number_t::operator long() const {
  return _value;
}

number_t::operator std::string() const {
  return std::to_string(_value);
}

// comparisons
bool operator==(const number_t &num1, const number_t &num2) {
  return (num1._value == num2._value);
}

bool operator!=(const number_t &num1, const number_t &num2) {
  return (num1._value != num2._value);
}

bool operator<(const number_t &num1, const number_t &num2) {
  return (num1._value < num2._value);
}

bool operator>(const number_t &num1, const number_t &num2) {
  return (num1._value > num2._value);
}

bool operator<=(const number_t &num1, const number_t &num2) {
  return (num1._value <= num2._value);
}

bool operator>=(const number_t &num1, const number_t &num2) {
  return (num1._value >= num2._value);
}

// I/O
std::ostream &operator<<(std::ostream &stream, const number_t &num) {
  stream << num._value;
  return stream;
}
std::istream &operator>>(std::istream &stream, number_t &num) {
  stream >> num._value;
  return stream;
}

// binary operators
number_t number_t::operator+=(const number_t &num) {
  this->_value += num._value;
  return *this;
}

number_t number_t::operator-=(const number_t &num) {
  this->_value -= num._value;
  return *this;
}

number_t number_t::operator*=(const number_t &num) {
  this->_value *= num._value;
  return *this;
}

number_t number_t::operator/=(const number_t &num) {
  this->_value /= num._value;
  return *this;
}

number_t number_t::operator%=(const number_t &num) {
  this->_value %= num._value;
  return *this;
}

number_t number_t::operator^=(const number_t &num) {
  this->_value ^= num._value;
  return *this;
}

number_t number_t::operator&=(const number_t &num) {
  this->_value &= num._value;
  return *this;
}

number_t number_t::operator|=(const number_t &num) {
  this->_value |= num._value;
  return *this;
}

// friend unary operators
number_t operator+(const number_t &num1, const number_t &num2) {
  return number_t(num1._value + num2._value);
}

number_t operator-(const number_t &num1, const number_t &num2) {
  return number_t(num1._value - num2._value);
}

number_t operator*(const number_t &num1, const number_t &num2) {
  return number_t(num1._value * num2._value);
}

number_t operator/(const number_t &num1, const number_t &num2) {
  return number_t(num1._value / num2._value);
}

number_t operator%(const number_t &num1, const number_t &num2) {
  return number_t(num1._value % num2._value);
}

number_t operator^(const number_t &num1, const number_t &num2) {
  return number_t(num1._value ^ num2._value);
}

number_t operator&(const number_t &num1, const number_t &num2) {
  return number_t(num1._value & num2._value);
}

number_t operator|(const number_t &num1, const number_t &num2) {
  return number_t(num1._value | num2._value);
}

// increment, decrement
number_t &number_t::operator++() {
  _value++;
  return *this;
}

number_t number_t::operator++(int) {
  number_t tmp = *this;
  ++*this;
  return tmp;
}

number_t &number_t::operator--() {
  _value--;
  return *this;
}

number_t number_t::operator--(int) {
  number_t tmp = *this;
  --*this;
  return tmp;
}

// unary operators
number_t number_t::operator-() const {
  return number_t(-_value);
}

number_t number_t::operator+() const {
  return number_t(+_value);
}

number_t number_t::operator!() const {
  return number_t(!_value);
}

number_t number_t::operator~() const {
  return number_t(~_value);
}

// binary shifts
number_t &number_t::operator<<=(const number_t &num) {
  _value <<= num._value;
  return *this;
}

number_t &number_t::operator>>=(const number_t &num) {
  _value >>= num._value;
  return *this;
}

number_t number_t::operator<<(const number_t &num) const {
  return number_t(_value << num._value);
}

number_t number_t::operator>>(const number_t &num) const {
  return number_t(_value >> num._value);
}