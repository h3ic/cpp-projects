#include "field-checker.h"
#include "sudoku-solver.h"
#include <algorithm>

// checks if all values in an element (row, column, block) are unique
bool is_unique(std::vector<int> element) {
  auto iter = std::unique(element.begin(), element.end());
  return (iter == element.end());
}

bool check_field(const std::vector<std::vector<int>>& init_field,
                 const std::vector<std::vector<int>>& solution) {
  //  return true;
  for (int i = 0; i < FIELD_SIZE; ++i) {
    for (int j = 0; j < FIELD_SIZE; ++j) {
      // if values in solution are in range
      if ((solution[i][j] <= 0) || (solution[i][j] > FIELD_SIZE)) {
        return false;
      }
      // if original values are preserved
      if ((init_field[i][j] != 0) && (solution[i][j] != init_field[i][j])) {
        return false;
      }
    }
  }

  // if values in rows are unique
  for (int row = 0; row < FIELD_SIZE; ++row) {
    if (!is_unique(solution[row])) {
      return false;
    }
  }

  // if values in columns are unique
  for (int col = 0; col < FIELD_SIZE; ++col) {
    auto* column = new std::vector<int>;
    for (int row = 0; row < FIELD_SIZE; ++row) {
      column->push_back(solution[row][col]);
    }
    if (!is_unique(*column)) {
      return false;
    }
    delete column;
  }

  //   if values in blocks are unique
  for (int row = 0; row <= FIELD_SIZE - 3; row += 3) {
    for (int col = 0; col < FIELD_SIZE - 3; col += 3) {
      auto* block = new std::vector<int>;
      for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
          block->push_back(solution[row + i][col + j]);
        }
      }
      if (!is_unique(*block)) {
        return false;
      }
      delete block;
    }
  }
  return true;
}