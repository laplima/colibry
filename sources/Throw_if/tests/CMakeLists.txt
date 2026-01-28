cmake_minimum_required(VERSION 3.30)

project(tests)

set(sources throw_if_test.cpp)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_BUILD_TYPE Debug)

find_package(Catch2 3 REQUIRED)

# INCLUDES
include_directories("$ENV{COLIBRY}/include")
add_compile_options(-Wall -Wextra -Wconversion -Wshadow)

add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} Catch2::Catch2WithMain)
