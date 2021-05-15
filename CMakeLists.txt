cmake_minimum_required(VERSION 3.19)
project(hw_02)

set(CMAKE_CXX_STANDARD 17)
include_directories(include)

add_executable(hw_02 src/main.cpp src/huffman.cpp include/huffman.h)
add_executable(hw_02_test test/test.cpp test/doctest.h include/huffman.h src/huffman.cpp)
