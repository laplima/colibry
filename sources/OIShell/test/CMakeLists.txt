cmake_minimum_required(VERSION 3.17)

project (tt)

set(sources
	../OIShell.cpp
	main.cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

set(libs readline)
set(mylibs)

foreach(lib ${libs})
	set(tlib ${lib}-NOTFOUND)
	find_library(tlib ${lib} PATHS /opt/local/lib)
	set(mylibs ${mylibs} ${tlib})
endforeach(lib)

include_directories(".." /opt/local/include)

# add_compile_options()

add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} ${mylibs})
