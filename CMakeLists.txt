cmake_minimum_required(VERSION 3.19)
project(textrank)

set(CMAKE_CXX_STANDARD 11)

INCLUDE_DIRECTORIES(lib/cppjieba/include lib/cppjieba/deps)

AUX_SOURCE_DIRECTORY(./src DIR_SRCS)

add_executable(textrank ${DIR_SRCS})
add_executable(segment main.cpp ./src/text_utils.cpp)