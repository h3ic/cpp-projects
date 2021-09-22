#pragma once

#include <utility>
#include <vector>

static constexpr size_t FIELD_SIZE = 9;

std::pair<size_t, std::vector<std::vector<int>>> sudoku_solve(const std::vector<std::vector<int>> &field);
