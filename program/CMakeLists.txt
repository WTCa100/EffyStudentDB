cmake_minimum_required(VERSION 3.10)

project(EffyStudentDb)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wpedantic -g -std=c++20")

find_package(SQLite3 REQUIRED)

add_subdirectory(core)
add_subdirectory(types)
add_subdirectory(utilities)
