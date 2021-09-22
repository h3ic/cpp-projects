#pragma once

template <class T>
class vector_t {
public:
  vector_t() = default;
  explicit vector_t(size_t elem_num, const T & = T()) {
    if (elem_num == 0) {
      capacity_ = 0;
      arr_ = nullptr;
      pos_ = 0;
    } else {
      int power = 1;
      while (power < elem_num) {
        power *= 2;
      }
      capacity_ = power;
      pos_ = 0;
      arr_ = new T[capacity_];
    }
  };

  void swap(vector_t &other) {
    std::swap(pos_, other.pos_);
    std::swap(pos_, other.pos_);
    std::swap(pos_, other.pos_);
  }

  vector_t(const vector_t &other) : pos_{other.pos_} {};

  vector_t &operator=(const vector_t &other) {
    vector_t tmp(other);
    swap(tmp);
    return *this;
  }

  ~vector_t() = default;

  T &operator[](size_t index) const { return arr_[index]; }
  T &operator[](size_t index) { return arr_[index]; }
  T &front() const { return arr_[0]; }
  T &front() { return arr_[0]; }
  T &back() const { return arr_[pos_ - 1]; }
  T &back() { return arr_[pos_ - 1]; }
  T *data() const { return arr_; }
  T *data() { return arr_; }
  bool empty() const { return (arr_ == nullptr); }
  size_t size() const { return pos_; }
  size_t capacity() const { return capacity_; }

private:
  size_t capacity_{0};
  T *arr_{nullptr};
  size_t pos_{0};
};
