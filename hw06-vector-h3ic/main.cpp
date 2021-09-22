#include "gtest/gtest.h"

#include <array>
#include <chrono>
#include <memory>
#include <random>
#include <vector>

#include "vector.h"

template <typename T>
struct action_interface_t {
  virtual void operator()(std::vector<T> &v) const = 0;
  virtual void operator()(vector_t<T> &v) const = 0;
  virtual void to_buffer(std::ostream &buffer) const = 0;
};

template <typename T>
struct clear_t : action_interface_t<T> {
  void operator()(std::vector<T> &v) const override { v.clear(); }
  void operator()(vector_t<T> &v) const override { v.clear(); }
  void to_buffer(std::ostream &buffer) const override { buffer << "Clear"; }
};

template <typename T>
struct val_insert_t : action_interface_t<T> {
  val_insert_t(size_t pos, T val) : pos(pos), val(std::move(val)) {}
  void operator()(std::vector<T> &v) const override { v.insert(v.begin() + pos, val); }
  void operator()(vector_t<T> &v) const override { v.insert(pos, val); }
  void to_buffer(std::ostream &buffer) const override {
    buffer << "Insert value before position: " << pos << "  val: " << val;
  }
  size_t pos;
  T val;
};

template <typename T>
struct range_insert_t : action_interface_t<T> {
  range_insert_t(size_t pos, size_t count, T val) : pos(pos), count(count), val(std::move(val)) {}
  void operator()(std::vector<T> &v) const override { v.insert(v.begin() + pos, count, val); }
  void operator()(vector_t<T> &v) const override { v.insert(pos, count, val); }
  void to_buffer(std::ostream &buffer) const override {
    buffer << "Insert range before position: " << pos << "  count: " << count << "  val: " << val;
  }
  size_t pos;
  size_t count;
  T val;
};

template <typename T>
struct erase_val_t : action_interface_t<T> {
  explicit erase_val_t(size_t pos) : pos(pos) {}
  void operator()(std::vector<T> &v) const override { v.erase(v.begin() + pos); }
  void operator()(vector_t<T> &v) const override { v.erase(pos); }
  void to_buffer(std::ostream &buffer) const override { buffer << "Erase value on positions: " << pos; }
  size_t pos;
};

template <typename T>
struct erase_range_t : action_interface_t<T> {
  erase_range_t(size_t first, size_t last) : first(first), last(last) {}
  void operator()(std::vector<T> &v) const override { v.erase(v.begin() + first, v.begin() + last); }
  void operator()(vector_t<T> &v) const override { v.erase(first, last); }
  void to_buffer(std::ostream &buffer) const override {
    buffer << "Erase values in range: [" << first << ", " << last << ")";
  }
  size_t first;
  size_t last;
};

template <typename T>
struct push_back_t : action_interface_t<T> {
  explicit push_back_t(T val) : val(std::move(val)) {}
  void operator()(std::vector<T> &v) const override { v.push_back(val); }
  void operator()(vector_t<T> &v) const override { v.push_back(val); }
  void to_buffer(std::ostream &buffer) const override { buffer << "Push back value: " << val; }
  T val;
};

template <typename T>
struct pop_back_t : action_interface_t<T> {
  void operator()(std::vector<T> &v) const override { v.pop_back(); }
  void operator()(vector_t<T> &v) const override { v.pop_back(); }
  void to_buffer(std::ostream &buffer) const override { buffer << "Pop back value"; }
};

template <typename T>
struct resize_t : action_interface_t<T> {
  explicit resize_t(size_t new_size) : new_size(new_size), val(std::nullopt) {}
  resize_t(size_t new_size, T val) : new_size(new_size), val(std::move(val)) {}
  void operator()(std::vector<T> &v) const override { val ? v.resize(new_size, *val) : v.resize(new_size); }
  void operator()(vector_t<T> &v) const override { val ? v.resize(new_size, *val) : v.resize(new_size); }
  void to_buffer(std::ostream &buffer) const override {
    buffer << "Resize to size: " << new_size << " with ";
    val ? (buffer << "value: " << *val) : (buffer << "default value");
  }
  size_t new_size;
  std::optional<T> val;
};

template <typename T>
bool check_equality(const std::vector<T> &expected, const vector_t<T> &actual, std::ostream &buffer) {
  const auto dump_on_error = [&](const std::string &error) {
    buffer << "ERROR!" << std::endl << error << std::endl;

    const auto dump = [&](const std::string &prefix, const auto &v) {
      buffer << "  " << prefix << " [sz: " << v.size() << "] { ";
      for (size_t i = 0; i < v.size(); ++i) {
        buffer << v[i] << " ";
      }
      buffer << "}" << std::endl;
    };

    dump("Expected:", expected);
    dump("Actual:  ", actual);
  };

  if (expected.size() != actual.size()) {
    dump_on_error("Different sizes");
    return false;
  }
  if (expected.empty() != actual.empty()) {
    dump_on_error("Different empty status");
    return false;
  }
  if (!expected.empty()) {
    if (expected.front() != actual.front()) {
      dump_on_error("Different front values");
      return false;
    }
    if (expected.back() != actual.back()) {
      dump_on_error("Different back values");
      return false;
    }
  }

  for (size_t i = 0; i < expected.size(); ++i) {
    if (expected[i] != actual[i]) {
      dump_on_error("Different values on position " + std::to_string(i));
      return false;
    }
  }
  for (size_t i = 0; i < actual.capacity(); ++i) {
    if (actual.data()[i] != actual[i]) {
      dump_on_error("Compare data and operator[] " + std::to_string(i));
    }
  }

  buffer << "OK!" << std::endl;
  return true;
}

template <typename T>
bool run_test(const std::vector<std::shared_ptr<action_interface_t<T>>> &ms) {
  std::stringstream buffer;
  std::vector<T> expected;
  vector_t<T> actual;

  const auto check = [&]() {
    bool res = check_equality(expected, actual, buffer);
    if (!res) {
      std::cout << buffer.str() << std::endl << std::endl;
    }
    return res;
  };

  buffer << "Init by default constructor\t";
  if (!check()) {
    return false;
  }

  for (const auto &action_ptr : ms) {
    action_interface_t<T> &action = *action_ptr;

    action(expected);
    action(actual);

    action.to_buffer(buffer);
    buffer << "\t";

    if (!check()) {
      return false;
    }
  }
  return true;
}

template <typename T>
struct action_factory_t {
  template <typename Action, typename... Args>
  std::shared_ptr<action_interface_t<T>> a(Args &&...args) const {
    return std::make_shared<Action>(std::forward<Args>(args)...);
  }
};

TEST(Vector, DefaultConstructor) {
  vector_t<int> v;

  ASSERT_EQ(v.size(), 0);
  ASSERT_EQ(v.capacity(), 0);
  ASSERT_EQ(v.data(), nullptr);
  ASSERT_TRUE(v.empty());
}

TEST(Vector, ValueConstructor) {
  static constexpr size_t count1 = 10;
  vector_t<std::string> v(count1, "abacaba");

  ASSERT_EQ(v.size(), count1);
  ASSERT_EQ(v.capacity(), 16);
  ASSERT_FALSE(v.empty());
  for (size_t i = 0; i < count1; ++i) {
    ASSERT_EQ(v[i], "abacaba");
  }

  vector_t<std::string> empty_v(0, "test_string");
  ASSERT_EQ(empty_v.size(), 0);
  ASSERT_EQ(empty_v.capacity(), 0);
  ASSERT_EQ(empty_v.data(), nullptr);
  ASSERT_TRUE(empty_v.empty());
}

TEST(Vector, CopyConstructor) {
  {
    vector_t<std::string> v(10, "a");

    vector_t<std::string> copy(v);
    ASSERT_EQ(v, copy);

    copy[0] = "b";
    ASSERT_NE(v, copy);
  }

  { // empty copy
    vector_t<int> v;

    vector_t<int> copy(v);
    ASSERT_TRUE(v.empty());
    ASSERT_EQ(v.size(), 0);
    ASSERT_EQ(v.data(), nullptr);
  }

  { // narrowing copy
    vector_t<std::string> v(10, "abacaba");
    ASSERT_EQ(v.capacity(), 16);

    v.erase(7, 10);
    ASSERT_EQ(v.capacity(), 16);
    ASSERT_EQ(v.size(), 7);

    vector_t<std::string> copy(v);
    ASSERT_EQ(copy.capacity(), 8);
    ASSERT_EQ(copy.size(), 7);
    ASSERT_EQ(copy, v);

    vector_t<std::string> empty_v;
    vector_t<std::string> empty_copy(empty_v);
    ASSERT_EQ(empty_copy.capacity(), 0);
    ASSERT_EQ(empty_copy.size(), 0);
    ASSERT_EQ(empty_copy.data(), nullptr);

    copy.erase(0, copy.size());
    ASSERT_TRUE(copy.empty());

    vector_t<std::string> empty_copy2(copy);
    ASSERT_EQ(empty_copy2.capacity(), 0);
    ASSERT_EQ(empty_copy2.size(), 0);
    ASSERT_EQ(empty_copy2.data(), nullptr);
  }
}

TEST(Vector, CopyAssignment) {
  { // self-assignment
    vector_t<std::string> v(10, "abacaba");
    v = v;
    ASSERT_EQ(v.size(), 10);
    ASSERT_EQ(v.capacity(), 16);
    for (size_t i = 0; i < 10; ++i) {
      ASSERT_EQ(v[i], "abacaba");
    }
  }
  {
    vector_t<std::string> v(5, "abacaba");
    {
      vector_t<std::string> tmp_v(10, "xxx");
      v = tmp_v;
    }
    ASSERT_EQ(v.size(), 10);
    ASSERT_EQ(v.capacity(), 16);
    for (size_t i = 0; i < 10; ++i) {
      ASSERT_EQ(v[i], "xxx");
    }
  }
  {                                         // narrowing assignment
    vector_t<std::string> v(10, "abacaba"); // 10
    v.erase(6, 10);                         // 6;
    ASSERT_EQ(v.size(), 6);
    ASSERT_EQ(v.capacity(), 16);

    vector_t<std::string> tmp_v(1, "xxx");
    tmp_v = v;
    ASSERT_EQ(tmp_v.size(), 6);
    ASSERT_EQ(tmp_v.capacity(), 8);
    ASSERT_NE(tmp_v.data(), v.data());
    for (size_t i = 0; i < 6; ++i) {
      ASSERT_EQ(tmp_v[i], "abacaba");
    }
  }
  { // empty assignment
    vector_t<std::string> empty_v;
    vector_t<std::string> v(10, "abacaba");
    v = empty_v;
    ASSERT_EQ(v.size(), 0);
    ASSERT_EQ(v.capacity(), 0);
    ASSERT_EQ(v.data(), nullptr);

    v = vector_t<std::string>(10, "abacaba");
    v.erase(0, 10);
    ASSERT_TRUE(v.empty());
    empty_v = v;
    ASSERT_EQ(empty_v.size(), 0);
    ASSERT_EQ(empty_v.capacity(), 0);
    ASSERT_EQ(empty_v.data(), nullptr);

    vector_t<std::string> tmp_v(5, "xxx");
    v = tmp_v;
    ASSERT_EQ(v.size(), 5);
    ASSERT_EQ(v.capacity(), 8);
    ASSERT_NE(v.data(), tmp_v.data());
    for (size_t i = 0; i < 5; ++i) {
      ASSERT_EQ(v[i], "xxx");
    }
  }
}

TEST(Vector, BracketOperator) {
  vector_t<int> v(5, 10);
  v[0] = 12;
  ASSERT_EQ(v[0], 12);
  ASSERT_EQ(v[1], 10);

  const vector_t<int> &v_const = v;
  ASSERT_EQ(v_const[0], 12);
  ASSERT_EQ(v_const[1], 10);
}

TEST(Vector, Front) {
  vector_t<int> v(5, 10);
  const vector_t<int> &v_const = v;
  ASSERT_EQ(v.front(), 10);
  ASSERT_EQ(v_const.front(), 10);
  v.front() = 12;
  ASSERT_EQ(v.front(), 12);
  ASSERT_EQ(v_const.front(), 12);
  v[0] = 15;
  ASSERT_EQ(v.front(), 15);
  ASSERT_EQ(v_const.front(), 15);
}

TEST(Vector, Back) {
  vector_t<int> v(5, 10);
  const vector_t<int> &v_const = v;
  ASSERT_EQ(v.back(), 10);
  ASSERT_EQ(v_const.back(), 10);
  v.back() = 12;
  ASSERT_EQ(v.back(), 12);
  ASSERT_EQ(v_const.back(), 12);
  v[4] = 15;
  ASSERT_EQ(v.back(), 15);
  ASSERT_EQ(v_const.back(), 15);
}

TEST(Vector, Data) {
  vector_t<int> v(10, 5);
  const vector_t<int> &v_const = v;
  for (int i = 0; i < 10; ++i) {
    v[i] = i;
  }

  int *data = v.data();
  const int *data_const = v_const.data();
  for (int i = 0; i < 10; ++i) {
    ASSERT_EQ(v[i], i);
    ASSERT_EQ(v_const[i], i);
    ASSERT_EQ(data[i], i);
    ASSERT_EQ(data_const[i], i);
  }

  for (int i = 0; i < 10; ++i) {
    data[9 - i] = i;
  }
  for (int i = 0; i < 10; ++i) {
    ASSERT_EQ(v[9 - i], i);
    ASSERT_EQ(v_const[9 - i], i);
    ASSERT_EQ(data[9 - i], i);
    ASSERT_EQ(data_const[9 - i], i);
  }
}

TEST(Vector, Empty) {
  vector_t<std::string> v;
  ASSERT_TRUE(v.empty());

  v.push_back("a");
  v.push_back("b");
  ASSERT_FALSE(v.empty());
  v.pop_back();
  v.pop_back();
  ASSERT_TRUE(v.empty());
}

TEST(Vector, Size) {
  vector_t<int> v(5, 10);
  ASSERT_EQ(v.size(), 5);

  for (int i = 0; i < 5; ++i) {
    v.pop_back();
    ASSERT_EQ(v.size(), 4 - i);
  }

  for (int i = 0; i < 5; ++i) {
    v.push_back(1);
    ASSERT_EQ(v.size(), i + 1);
  }
}

TEST(Vector, ResizeExtend) {
  action_factory_t<std::string> f;
  ASSERT_TRUE(run_test(std::vector{f.a<push_back_t<std::string>>("a"), f.a<push_back_t<std::string>>("b"),
                                   f.a<push_back_t<std::string>>("c"), f.a<resize_t<std::string>>(4, "e"),
                                   f.a<resize_t<std::string>>(20)}));

  action_factory_t<int> df;
  ASSERT_TRUE(run_test(std::vector{df.a<resize_t<int>>(10, 20), df.a<resize_t<int>>(100)}));
}

TEST(Vector, ResizeNarrow) {
  action_factory_t<std::string> f;
  ASSERT_TRUE(run_test(std::vector{
      f.a<range_insert_t<std::string>>(0, 100, "abacaba"),
      f.a<resize_t<std::string>>(70),
      f.a<resize_t<std::string>>(10, "a"),
      f.a<resize_t<std::string>>(0),
  }));

  action_factory_t<int> df;
  ASSERT_TRUE(run_test(std::vector{
      df.a<range_insert_t<int>>(0, 100, 12),
      df.a<resize_t<int>>(70),
      df.a<resize_t<int>>(10, 22),
      df.a<resize_t<int>>(0),
  }));
}

static std::mt19937_64 &get_gen() {
  static std::seed_seq seq = {std::chrono::high_resolution_clock::now().time_since_epoch().count()};
  static std::mt19937_64 gen(seq);
  return gen;
}
static bool rnd_bool(double p = 0.5) {
  return std::bernoulli_distribution(p)(get_gen());
}
template <typename T>
static T rnd(T a = std::numeric_limits<T>::min(), T b = std::numeric_limits<T>::max()) {
  return std::uniform_int_distribution<T>(a, b)(get_gen());
}
static std::string rnd_str(size_t min_len = 0, size_t max_len = 5) {
  std::string res;
  size_t len = rnd<size_t>(min_len, max_len);
  res.reserve(len);
  for (size_t i = 0; i < len; ++i) {
    res.push_back(rnd<char>('a', 'z'));
  }
  return res;
}

static constexpr size_t MAX_SIZE = 100'000;

template <typename T, typename Gen>
static bool mixed_resize_test(size_t count, const Gen &g) {
  action_factory_t<T> f;
  size_t cur_size = 0;
  std::vector<std::shared_ptr<action_interface_t<T>>> ms;
  for (; count > 0; --count) {
    bool extend = cur_size == 0 || (rnd_bool() && cur_size * 2 <= MAX_SIZE);

    if (extend) {
      cur_size = rnd_bool() ? cur_size * 2 : rnd<size_t>(cur_size, 2 * cur_size);
    } else {
      cur_size = rnd_bool() ? cur_size / 2 : rnd<size_t>(0, cur_size);
    }

    ms.push_back(rnd_bool() ? f.template a<resize_t<T>>(cur_size) : f.template a<resize_t<T>>(cur_size, g()));
  }
  return run_test(ms);
}

TEST(Vector, ResizeMixed) {
  ASSERT_TRUE(mixed_resize_test<std::string>(1000, []() { return rnd_str(); }));
  ASSERT_TRUE(mixed_resize_test<int>(1000, []() { return rnd<int>(-100, 100); }));
}

TEST(Vector, Capacity) {
  vector_t<int> v;
  ASSERT_EQ(v.capacity(), 0);

  v.push_back(1);
  ASSERT_EQ(v.capacity(), 1);

  v.push_back(2);
  ASSERT_EQ(v.capacity(), 2);

  v.push_back(3);
  ASSERT_EQ(v.capacity(), 4);
  v.push_back(4);
  ASSERT_EQ(v.capacity(), 4);

  for (size_t i = 0; i < 4; ++i) {
    v.push_back(5 + i);
    ASSERT_EQ(v.capacity(), 8);
  }

  v.resize(30, 12);
  ASSERT_EQ(v.capacity(), 32);
  v.resize(32, 13); // 32
  ASSERT_EQ(v.capacity(), 32);

  v.insert(5, 20, 10); // 52
  ASSERT_EQ(v.capacity(), 64);
  for (size_t i = 0; i < 13; ++i) {
    ASSERT_EQ(v.capacity(), 64);
    v.insert(7, 12);
  }
  ASSERT_EQ(v.capacity(), 128); // 65

  v.pop_back(); // 64
  ASSERT_EQ(v.capacity(), 128);

  for (size_t i = 0; i < 33; ++i) {
    v.erase(0);
  }
  // 31
  ASSERT_EQ(v.capacity(), 128);

  v.erase(0, 16); // 15

  ASSERT_EQ(v.capacity(), 128);
  v.pop_back();
  v.pop_back();
  v.pop_back(); // 12
  ASSERT_EQ(v.capacity(), 128);

  v.shrink_to_fit();
  ASSERT_EQ(v.capacity(), 16);

  v.clear();
  ASSERT_EQ(v.capacity(), 0);
  ASSERT_EQ(v.data(), nullptr);

  v.push_back(10); // 1
  v.pop_back();    // 0
  ASSERT_EQ(v.capacity(), 1);

  v.shrink_to_fit();
  ASSERT_EQ(v.capacity(), 0);
  ASSERT_EQ(v.data(), nullptr);

  v.insert(0, 100, 12); // 128
  ASSERT_EQ(v.capacity(), 128);

  v.shrink_to_fit();
  ASSERT_EQ(v.capacity(), 128);

  v.resize(10); // 10
  ASSERT_EQ(v.capacity(), 128);

  v.resize(0); // 0
  ASSERT_EQ(v.capacity(), 128);
}

#define CHECK_RESERVE_CAPACITY(cap, sz)                                                                                \
  do {                                                                                                                 \
    ASSERT_EQ(v.capacity(), (cap));                                                                                    \
    ASSERT_EQ(v.size(), (sz));                                                                                         \
    ASSERT_NE(v.data(), nullptr);                                                                                      \
    size_t _sz = (sz);                                                                                                 \
    for (size_t i = 0; i < _sz; ++i) {                                                                                 \
      ASSERT_EQ(v[i], "abacaba");                                                                                      \
    }                                                                                                                  \
  } while (false)

TEST(Vector, Reserve) {
  { // empty vector
    vector_t<std::string> v;
    ASSERT_EQ(v.capacity(), 0);
    ASSERT_EQ(v.size(), 0);
    ASSERT_EQ(v.data(), nullptr);

    v.reserve(10);
    CHECK_RESERVE_CAPACITY(16, 0);

    v.reserve(3);
    CHECK_RESERVE_CAPACITY(16, 0);

    v.reserve(100);
    CHECK_RESERVE_CAPACITY(128, 0);

    v.reserve(256);
    CHECK_RESERVE_CAPACITY(256, 0);

    v.reserve(0);
    CHECK_RESERVE_CAPACITY(256, 0);
  }
  { // non-empty vector
    vector_t<std::string> v(15, "abacaba");
    CHECK_RESERVE_CAPACITY(16, 15);

    v.reserve(32);
    CHECK_RESERVE_CAPACITY(32, 15);

    for (size_t i = 0; i < 32 - 15; ++i) {
      v.push_back("abacaba");
    }
    CHECK_RESERVE_CAPACITY(32, 32);

    v.reserve(16);
    CHECK_RESERVE_CAPACITY(32, 32);

    v.reserve(0);
    CHECK_RESERVE_CAPACITY(32, 32);

    v.insert(14, "abacaba");
    CHECK_RESERVE_CAPACITY(64, 33);

    v.reserve(1023);
    CHECK_RESERVE_CAPACITY(1024, 33);
  }
}

#undef CHECK_RESERVE_CAPACITY

TEST(Vector, Clear) {
  vector_t<std::string> empty_v;

  vector_t<std::string> v(10, "20");
  ASSERT_NE(empty_v, v);

  v.clear();
  ASSERT_TRUE(v.empty());
  ASSERT_EQ(v.size(), 0);
  ASSERT_EQ(v.data(), nullptr);
  ASSERT_EQ(v, empty_v);
}

template <typename T, typename Gen>
static bool random_insert_test(size_t count, const Gen &g) {
  action_factory_t<T> f;
  std::vector<std::shared_ptr<action_interface_t<T>>> ms;
  size_t cur_size = 0;
  for (size_t i = 0; i < count; ++i) {
    size_t insert_before = rnd<size_t>(0, cur_size);
    size_t cnt = rnd<size_t>(0, 10);
    T val = g();
    if (rnd_bool()) {
      ms.push_back(f.template a<range_insert_t<T>>(insert_before, cnt, val));
      cur_size += cnt;
    } else {
      ms.push_back(f.template a<val_insert_t<T>>(insert_before, val));
      cur_size += 1;
    }
  }

  return run_test(ms);
}

TEST(Vector, InsertSimple) {
  action_factory_t<std::string> f;
  ASSERT_TRUE(run_test(std::vector{
      f.a<range_insert_t<std::string>>(0, 10, "abacaba"), // 10
      f.a<val_insert_t<std::string>>(0, "a"),             // 11
      f.a<val_insert_t<std::string>>(11, "b"),            // 12
      f.a<val_insert_t<std::string>>(12, "c"),            // 13
      f.a<range_insert_t<std::string>>(13, 100, "d"),     // 113
      f.a<range_insert_t<std::string>>(0, 10, "e"),       // 123
  }));
}

TEST(Vector, InsertRandom) {
  ASSERT_TRUE(random_insert_test<std::string>(1000, []() { return rnd_str(); }));
  ASSERT_TRUE(random_insert_test<int>(1000, []() { return rnd<int>(); }));
}

template <typename T>
static bool random_erase_test(size_t count, const T &val) {
  action_factory_t<T> f;
  std::vector<std::shared_ptr<action_interface_t<T>>> ms;
  ms.push_back(f.template a<range_insert_t<T>>(0, MAX_SIZE, val));
  size_t cur_size = MAX_SIZE;

  for (size_t i = 0; i < count; ++i) {
    size_t first = rnd<size_t>(0, cur_size);
    size_t last = rnd<size_t>(first, cur_size);
    bool is_range = cur_size == 0 || rnd_bool();
    ms.push_back(is_range ? f.template a<erase_range_t<T>>(first, last)
                          : f.template a<erase_val_t<T>>(rnd<size_t>(0, cur_size - 1)));
    cur_size -= (is_range ? last - first : 1);
  }

  return run_test(ms);
}

TEST(Vector, EraseSimple) {
  action_factory_t<int> f;
  std::vector<std::shared_ptr<action_interface_t<int>>> ms;

  for (size_t i = 0; i < 100; ++i) {
    ms.push_back(f.a<push_back_t<int>>(i));
  }

  std::vector<std::shared_ptr<action_interface_t<int>>> main_ms = {
      // 100
      f.a<erase_val_t<int>>(0),  // 99
      f.a<erase_val_t<int>>(98), // 98
      f.a<erase_val_t<int>>(50), // 97

      f.a<erase_range_t<int>>(0, 0),   // 97
      f.a<erase_range_t<int>>(97, 97), // 97
      f.a<erase_range_t<int>>(50, 50), // 97
      f.a<erase_range_t<int>>(96, 97), // 96
      f.a<erase_range_t<int>>(0, 1),   // 95
      f.a<erase_range_t<int>>(0, 10),  // 85
      f.a<erase_range_t<int>>(75, 85), // 75
      f.a<erase_range_t<int>>(30, 40), // 65
      f.a<erase_range_t<int>>(0, 65),  // 0
  };

  main_ms.insert(main_ms.begin(), ms.begin(), ms.end());
  ASSERT_TRUE(run_test(main_ms));
}

TEST(Vector, EraseRandom) {
  ASSERT_TRUE(random_erase_test<std::string>(1000, "abacaba"));
  ASSERT_TRUE(random_erase_test<int>(1000, 12));
}

template <typename T, typename Gen>
bool random_changes_test(size_t count, const Gen &g) {
  action_factory_t<T> f;
  std::vector<std::shared_ptr<action_interface_t<T>>> ms;

  size_t cur_size = 0;
  for (size_t i = 0; i < count; ++i) {
    bool is_expand = cur_size == 0 || rnd_bool();

    size_t rnd_val = rnd<size_t>(0, 3);
    if (is_expand) {
      size_t insert_before = rnd<size_t>(0, cur_size);
      size_t cnt = rnd<size_t>(0, 20);

      switch (rnd_val) {
      case 0: {
        ms.push_back(f.template a<push_back_t<T>>(g()));
        cur_size += 1;
        continue;
      }
      case 1: {
        ms.push_back(f.template a<range_insert_t<T>>(insert_before, cnt, g()));
        cur_size += cnt;
        continue;
      }
      case 2: {
        ms.push_back(f.template a<val_insert_t<T>>(insert_before, g()));
        cur_size += 1;
        continue;
      }
      case 3: {
        ms.push_back(f.template a<resize_t<T>>(cur_size + cnt, g()));
        cur_size += cnt;
        continue;
      }
      default:
        assert(false);
      }
    } else {
      if (rnd_bool(0.02)) {
        ms.push_back(f.template a<clear_t<T>>());
        cur_size = 0;
        continue;
      }

      size_t first = rnd<size_t>(0, cur_size);
      size_t last = first + rnd<size_t>(0, std::min<size_t>(10, cur_size - first));
      switch (rnd_val) {
      case 0: {
        ms.push_back(f.template a<pop_back_t<T>>());
        cur_size -= 1;
        continue;
      }
      case 1: {
        ms.push_back(f.template a<erase_val_t<T>>(rnd<size_t>(0, cur_size - 1)));
        cur_size -= 1;
        continue;
      }
      case 2: {
        ms.push_back(f.template a<erase_range_t<T>>(first, last));
        cur_size -= (last - first);
        continue;
      }
      case 3: {
        ms.push_back(f.template a<resize_t<T>>(cur_size + first - last));
        cur_size -= (last - first);
        continue;
      }
      default:
        assert(false);
      }
    }
  }
  return run_test(ms);
}

TEST(Vector, ChangesMixed) {
  ASSERT_TRUE(random_changes_test<int>(100'000, []() { return rnd<int>(); }));
  ASSERT_TRUE(random_changes_test<std::string>(50'000, []() { return rnd_str(0, 10); }));
}

TEST(Vector, SwapEmpty) {
  vector_t<int> a(10, 10);
  const int *a_data = a.data();
  vector_t<int> b;

  a.swap(b);

  ASSERT_EQ(a.capacity(), 0);
  ASSERT_EQ(b.capacity(), 16);

  ASSERT_EQ(a.size(), 0);
  ASSERT_EQ(b.size(), 10);

  ASSERT_EQ(a.data(), nullptr);
  ASSERT_EQ(b.data(), a_data);
}

TEST(Vector, SwapNonEmpty) {
  vector_t<int> a(10, 10);
  const int *a_data = a.data();
  vector_t<int> b(5, 5);
  const int *b_data = b.data();

  a.swap(b);

  ASSERT_EQ(a.capacity(), 8);
  ASSERT_EQ(b.capacity(), 16);

  ASSERT_EQ(a.size(), 5);
  ASSERT_EQ(b.size(), 10);

  ASSERT_EQ(a.data(), b_data);
  ASSERT_EQ(b.data(), a_data);
}

TEST(Vector, SwapStress) {
  static constexpr size_t count = 1'000'000;
  static constexpr size_t iterations = 1'000'000;
  std::array<vector_t<int>, 2> vs = {vector_t<int>(count, 10), vector_t<int>(3 * count, 20)};
  std::array<std::tuple<size_t, size_t, int>, 2> info{
      std::tuple<size_t, size_t, int>{1024UL * 1024, count, 10},
      std::tuple<size_t, size_t, int>{4UL * 1024 * 1024, 3 * count, 20}};

  for (size_t i = 0; i < iterations; ++i) {
    size_t pos = rnd<size_t>(0, 1);
    vs[pos].swap(vs[1 - pos]);

    std::swap(info[0], info[1]);

    for (size_t i = 0; i < 2; ++i) {
      auto [cap, sz, val] = info[i];
      ASSERT_EQ(vs[i].capacity(), cap);
      ASSERT_EQ(vs[i].size(), sz);
      ASSERT_EQ(vs[i].front(), val);
    }
  }
}

#define CHECK_COMPARISIONS_IMPL(a, b, la, lb)                                                                          \
  {                                                                                                                    \
    bool a_less_b = (a) < (b);                                                                                         \
    ASSERT_EQ(a_less_b, (la));                                                                                         \
    bool b_less_a = (b) < (a);                                                                                         \
    ASSERT_EQ(b_less_a, (lb));                                                                                         \
    ASSERT_EQ((a) == (b), !(a_less_b || b_less_a));                                                                    \
    ASSERT_EQ((a) != (b), a_less_b || b_less_a);                                                                       \
    ASSERT_EQ((a) > (b), b_less_a);                                                                                    \
    ASSERT_EQ((a) <= (b), !b_less_a);                                                                                  \
    ASSERT_EQ((a) >= (b), !a_less_b);                                                                                  \
  }

#define CHECK_COMPARISIONS(a, b, la, lb) CHECK_COMPARISIONS_IMPL(a, b, la, lb) CHECK_COMPARISIONS_IMPL(b, a, lb, la)

TEST(Vector, Comparisions) {
  {
    CHECK_COMPARISIONS(vector_t<int>(), vector_t<int>(), false, false)

    vector_t<int> a;
    vector_t<int> b;
    CHECK_COMPARISIONS(a, b, false, false)

    vector_t<std::string> c(5, "abacaba");
    CHECK_COMPARISIONS(c, vector_t<std::string>(5, "abacaba"), false, false)

    CHECK_COMPARISIONS(vector_t<std::string>(), c, true, false)
  }
  {
    vector_t<size_t> a;
    for (size_t i = 0; i < 16; ++i) {
      a.push_back(i);
    }
    vector_t<size_t> b(a);

    CHECK_COMPARISIONS(a, b, false, false)

    a.push_back(16); // 17
    CHECK_COMPARISIONS(a, b, false, true)

    a.erase(15, 17); // 15
    CHECK_COMPARISIONS(a, b, true, false);

    a.push_back(10); // 16
    CHECK_COMPARISIONS(a, b, true, false);

    a.back() = 20;
    CHECK_COMPARISIONS(a, b, false, true);
  }
}

TEST(Vector, ComparisionsFull) {
  static constexpr size_t mask_len = 7;

  const auto gen = [](size_t len, uint32_t mask) -> std::pair<vector_t<int>, std::vector<int>> {
    vector_t<int> res;
    for (size_t i = 0; i < len; ++i) {
      res.push_back((mask & (1U << i)) ? 1 : 0);
    }
    return {res, std::vector<int>(res.data(), res.data() + res.size())};
  };

  for (size_t len_a = 0; len_a <= mask_len; ++len_a) {
    for (uint32_t mask_a = 0; mask_a < (1U << len_a); ++mask_a) {
      auto [a, ea] = gen(len_a, mask_a);

      for (size_t len_b = 0; len_b <= mask_len; ++len_b) {
        for (uint32_t mask_b = 0; mask_b < (1U << len_b); ++mask_b) {
          auto [b, eb] = gen(len_b, mask_b);

          CHECK_COMPARISIONS(a, b, ea < eb, eb < ea);
        }
      }
    }
  }
}

TEST(Vector, ComparisionsRandom) {
  static constexpr size_t iterations = 10'000;
  static constexpr size_t max_len = 5;

  const auto gen = []() -> std::pair<vector_t<int>, std::vector<int>> {
    size_t len = rnd<size_t>(0, max_len);
    vector_t<int> v;
    for (size_t i = 0; i < len; ++i) {
      v.push_back(rnd<int>(0, 1));
    }
    return {v, std::vector<int>(v.data(), v.data() + v.size())};
  };

  for (size_t i = 0; i < iterations; ++i) {
    auto [a, ea] = gen();
    auto [b, eb] = gen();

    CHECK_COMPARISIONS(a, b, ea < eb, eb < ea);
  }
}

TEST(Vector, PushBackStress) {
  static constexpr size_t iterations = 10'000'000;
  vector_t<size_t> v;
  for (size_t i = 0; i < iterations; ++i) {
    v.push_back(i);
  }
}

TEST(Vector, PopBackStress) {
  static constexpr size_t iterations = 10'000'000;
  vector_t<size_t> v(iterations, 0);
  for (size_t i = 0; i < iterations; ++i) {
    v[i] = i;
  }

  for (size_t i = 0; i < iterations; ++i) {
    v.pop_back();
  }
}
