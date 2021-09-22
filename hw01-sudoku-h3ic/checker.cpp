#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>

#include "field-checker.h"
#include "sudoku-solver.h"

std::ostream &operator<<(std::ostream &stream, const std::vector<std::vector<int>> &field) {
  for (const auto &line : field) {
    for (int cell : line) {
      stream << cell << " ";
    }
    stream << std::endl;
  }
  return stream;
}

int main(int argc, char **argv) {
  assert(argc == 3);

  std::ifstream test_data(argv[1]);
  std::ifstream answer_data(argv[2]);

  std::vector<std::vector<int>> field(FIELD_SIZE, std::vector<int>(FIELD_SIZE, 0));
  for (auto &line : field) {
    for (int &cell : line) {
      test_data >> cell;
    }
  }

  size_t answers_count = 0;
  answer_data >> answers_count;

  std::pair<size_t, std::vector<std::vector<int>>> actual_answer = sudoku_solve(field);

  if (answers_count != actual_answer.first) {
    std::cerr << "Actual solutions count " << actual_answer.first << " isn't equals to " << answers_count << std::endl;
    return 1;
  }

  if (!check_field(field, actual_answer.second)) {
    std::cerr << "Actual solution: " << std::endl;
    std::cerr << actual_answer.second;
    std::cerr << "isn't solution for init field: " << std::endl;
    std::cerr << field;
    return 1;
  }
  return 0;
}
