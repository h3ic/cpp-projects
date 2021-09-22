#pragma once

#include <cstdint>
#include <iostream>

class number_t {
public:
  number_t();
  number_t(long); // implicit conv
  void swap(number_t &);
  number_t &operator=(const number_t &);

  explicit operator bool() const;
  explicit operator long() const;
  explicit operator std::string() const;

  friend bool operator==(const number_t &, const number_t &);
  friend bool operator!=(const number_t &, const number_t &);
  friend bool operator<(const number_t &, const number_t &);
  friend bool operator>(const number_t &, const number_t &);
  friend bool operator<=(const number_t &, const number_t &);
  friend bool operator>=(const number_t &, const number_t &);

  friend std::ostream &operator<<(std::ostream &, const number_t &);
  friend std::istream &operator>>(std::istream &, number_t &);

  number_t operator+=(const number_t &);
  number_t operator-=(const number_t &);
  number_t operator*=(const number_t &);
  number_t operator/=(const number_t &);
  number_t operator%=(const number_t &);
  number_t operator^=(const number_t &);
  number_t operator&=(const number_t &);
  number_t operator|=(const number_t &);

  friend number_t operator+(const number_t &, const number_t &);
  friend number_t operator-(const number_t &, const number_t &);
  friend number_t operator*(const number_t &, const number_t &);
  friend number_t operator/(const number_t &, const number_t &);
  friend number_t operator%(const number_t &, const number_t &);
  friend number_t operator^(const number_t &, const number_t &);
  friend number_t operator&(const number_t &, const number_t &);
  friend number_t operator|(const number_t &, const number_t &);

  number_t &operator++();
  number_t operator++(int);
  number_t &operator--();
  number_t operator--(int);

  number_t operator-() const;
  number_t operator+() const;
  number_t operator!() const;
  number_t operator~() const;

  number_t &operator<<=(const number_t &);
  number_t &operator>>=(const number_t &);
  number_t operator<<(const number_t &) const;
  number_t operator>>(const number_t &) const;

private:
  long _value{0};
};