#include "phone-book.h"

bool phone_book_t::create_user(const std::string &number, const std::string &name) {
  for (auto const &user_info : users_info) {
    if (user_info.user.number == number) {
      return false;
    }
  }
  user_info_t user_info;
  user_info.user.number = number;
  user_info.user.name = name;
  users_info.push_back(user_info);
  return true;
}

bool phone_book_t::add_call(const call_t &call) {
  auto call_number = call.number;
  for (auto &user_info : users_info) {
    if (user_info.user.number == std::string(call_number)) {
      calls.push_back(call);
      user_info.total_call_duration_s += call.duration_s;
      return true;
    }
  }
  return false;
}

std::vector<call_t> phone_book_t::get_calls(size_t start_pos, size_t count) const {
  size_t calls_size = calls.size();
  size_t chunk = start_pos + count;
  if (chunk <= calls_size and count > 0) {
    return std::vector<call_t>(calls.begin() + int(start_pos), calls.begin() + int(chunk));
  } else if (chunk > calls_size and start_pos < calls_size) {
    return std::vector<call_t>(calls.begin() + int(start_pos), calls.end());
  } else {
    return std::vector<call_t>{};
  }
}

struct compare_by_number {
  bool operator()(const user_info_t &info1, const user_info_t &info2) const {
    return (info1.total_call_duration_s > info2.total_call_duration_s) ||
           ((info1.total_call_duration_s == info2.total_call_duration_s) && (info1.user.name < info2.user.name)) ||
           ((info1.total_call_duration_s == info2.total_call_duration_s) && (info1.user.name == info2.user.name) &&
            (info1.user.number < info2.user.number));
  }
};

std::vector<user_info_t> phone_book_t::search_users_by_number(const std::string &number_prefix, size_t count) const {
  std::vector<user_info_t> result = {};
  std::vector<user_info_t> users_info_sorted = users_info;
  std::sort(users_info_sorted.begin(), users_info_sorted.end(), compare_by_number());
  for (const auto &contact : users_info_sorted) {
    if (contact.user.number.rfind(number_prefix, 0) == 0) {
      result.push_back(contact);
    }
  }
  if (result.size() <= count) {
    return result;
  } else {
    return std::vector<user_info_t>(result.begin(), result.begin() + int(count));
  }
}

struct compare_by_name {
  bool operator()(const user_info_t &info1, const user_info_t &info2) const {
    return (info1.user.name < info2.user.name) ||
           ((info1.user.name == info2.user.name) && (info1.total_call_duration_s > info2.total_call_duration_s)) ||
           ((info1.user.name == info2.user.name) && (info1.total_call_duration_s == info2.total_call_duration_s) &&
            (info1.user.number < info2.user.number));
  }
};

std::vector<user_info_t> phone_book_t::search_users_by_name(const std::string &name_prefix, size_t count) const {
  std::vector<user_info_t> result = {};
  std::vector<user_info_t> users_info_sorted = users_info;
  std::sort(users_info_sorted.begin(), users_info_sorted.end(), compare_by_name());
  for (const auto &contact : users_info_sorted) {
    if (contact.user.name.rfind(name_prefix, 0) == 0) {
      result.push_back(contact);
    }
  }
  if (result.size() <= count) {
    return result;
  } else {
    return std::vector<user_info_t>(result.begin(), result.begin() + int(count));
  }
}

void phone_book_t::clear() {
  users_info.clear();
  calls.clear();
}

size_t phone_book_t::size() const {
  return users_info.size();
}

bool phone_book_t::empty() const {
  return users_info.empty();
}