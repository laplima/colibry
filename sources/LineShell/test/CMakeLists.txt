cmake_minimum_required(VERSION 3.20)

# project name
project(lnjson)

# project sources
set(sources ../LineShell.cpp jishell-test.cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

# MYLIBS

# set(libs colibry)
set(libs fmt)

# additional os-specific libraries
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(libs ${libs})
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(libs ${libs})
endif()

set(MYLIBS)
foreach(lib ${libs})
	set(the_lib ${lib}-NOTFOUND)
	find_library(the_lib ${lib} PATHS "/opt/local/lib" "$ENV{COLIBRY}/lib")
	set(MYLIBS ${MYLIBS} ${the_lib})
endforeach(lib)

# INCLUDES

include_directories("/opt/local/include" "..")

# add_compile_options(_______________)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} ${MYLIBS})
