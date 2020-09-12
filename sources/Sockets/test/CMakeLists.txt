cmake_minimum_required(VERSION 3.14)

project(socket_tests)        # project name

set(execs
	byteorder
	echo_client
	echo_server
	mcast_echo_server
	udp_echo_server)

foreach(ex ${execs})
	set(${ex}-src ${ex}.cpp)
	# message("${ex}-src = ${${ex}-src}")
endforeach(ex)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

# MYLIBS

set(MYLIBS)
set(libs colibry)

# additional os-specific libraries
# if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
# 	set(libs ${libs} ____________)
# elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
# 	set(libs ${libs} ____________)
# endif()

foreach(lib ${libs})
	set(the_lib ${lib}-NOTFOUND)
	# non-standardised paths (e.g., "/opt/local/lib" "$ENV{<env_var>}/lib")
	find_library(the_lib ${lib} PATHS "/opt/local/lib" $ENV{COLIBRY}/lib)
	set(MYLIBS ${MYLIBS} ${the_lib})
endforeach(lib)

# INCLUDES

include_directories("/opt/local/include" $ENV{COLIBRY}/include)

# add_compile_options(____________)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/${BIN_DIR}")
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

foreach(proj ${execs})
	add_executable(${proj} ${${proj}-src})
	target_link_libraries(${proj} ${MYLIBS})
endforeach(proj)

# add_executable(${PROJECT_NAME} ${sources})
# target_link_libraries(${PROJECT_NAME} ${MYLIBS})