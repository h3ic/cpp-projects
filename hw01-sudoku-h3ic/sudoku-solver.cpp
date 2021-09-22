#include "sudoku-solver.h"
#include <set>

std::set<int> find_possible_values(const std::vector<std::vector<int>>& field,
                                   const std::pair<int, int>& empty_pos) {
  std::set<int> possible_values;
  // fill initial set with values 1-9
  for (int i = 1; i <= FIELD_SIZE; i++) {
    possible_values.insert(i);
  }
    int row = empty_pos.first;
    int col = empty_pos.second;

  // in rows and cols
  for (int i = 0; i < FIELD_SIZE; i++) {
    possible_values.erase(field[row][i]);
    possible_values.erase(field[i][col]);
  }

  // in blocks
  // left upper corner of a block
  int row_block_corner = row - row % 3;
  int col_block_corner = col - col % 3;

  for (int i = row_block_corner; i < row_block_corner + 3; i++) {
    for (int j = col_block_corner; j < col_block_corner + 3; j++) {
      possible_values.erase(field[i][j]);
    }
  }
  return possible_values;
}

std::pair<int, int>
find_empty_position(const std::vector<std::vector<int>>& field) {
  for (int row = 0; row < FIELD_SIZE; ++row) {
    for (int col = 0; col < FIELD_SIZE; ++col) {
      if (field[row][col] == 0) {
        return {row, col};
      }
    }
  }
  return {-1, -1};
}

bool is_complete(const std::vector<std::vector<int>>& field) {
  for (int row = 0; row < FIELD_SIZE; ++row) {
    for (int col = 0; col < FIELD_SIZE; ++col) {
      if (field[row][col] == 0) {
        return false;
      }
    }
  }
  return true;
}


size_t solve_recursively(std::vector<std::vector<int>> field) {
  if (is_complete(field)) {
    auto *solution = &field;
    return 1;
  }
  size_t sum = 0;
  const auto& empty_pos = find_empty_position(field);
  const auto& possible_values = find_possible_values(field, empty_pos);
  const int& row = empty_pos.first;
  const int& col = empty_pos.second;

  for (const auto& value : possible_values) {
    field[row][col] = value;
    sum += solve_recursively(field);
  }
  return sum;
}

std::pair<size_t, std::vector<std::vector<int>>>
sudoku_solve(const std::vector<std::vector<int>>& field) {
    std::vector<std::vector<int>> solution;
  // if initial field is full
  if (is_complete(field)) {
    return {1, field};
  }
  auto non_const_field = field;
  return {solve_recursively(non_const_field), *solution};
}