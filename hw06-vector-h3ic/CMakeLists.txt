cmake_minimum_required(VERSION 3.9)
project(vector)

set(CMAKE_CXX_STANDARD 17)

include(testing/CMakeLists.txt)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-sign-compare -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined,address,leak -fno-sanitize-recover=all -D_GLIBCXX_DEBUG")


set(SOURCES)
set(HEADERS vector.h)

set(TESTS main.cpp)

add_executable(tests ${SOURCES} ${HEADERS} ${TESTS})
target_link_libraries(tests gtest_main)
