cmake_minimum_required(VERSION 3.16)
project(hw01_sudoku)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES checker.cpp field-checker.cpp sudoku-solver.cpp)
set(HEADERS field-checker.h sudoku-solver.h)

add_executable(checker ${SOURCES} ${HEADERS})
