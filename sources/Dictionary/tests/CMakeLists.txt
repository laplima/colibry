cmake_minimum_required(VERSION 3.30)

project(dictest)

set(sources
	../Dictionary.cpp
	dic_tests.cpp)

find_package(Catch2 3 REQUIRED)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

# INCLUDES
include_directories("$ENV{COLIBRY}/include")
add_compile_options(-Wall -Wextra)

add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} Catch2::Catch2WithMain)
target_compile_options(${PROJECT_NAME} PUBLIC)
