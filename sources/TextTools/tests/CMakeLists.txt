cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

project(tt)

set(sources tttest.cpp)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_BUILD_TYPE Debug)

find_package(Catch2 3 REQUIRED)
find_package(colibry REQUIRED)

# add_compile_options(-Wall -Wshadow -Wextra -Wconversion)
add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} Catch2::Catch2WithMain colibry::colibry)

