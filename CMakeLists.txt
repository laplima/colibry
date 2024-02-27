#
# (C) 2018-20 by LAPLJ
#
cmake_minimum_required(VERSION 3.16)
project(colibry)
project(colibry_static)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 20)

set(fmt_DIR "/opt/local/lib/libfmt10/cmake/fmt")
find_package(fmt REQUIRED)

set(components
    SymTable
	Dictionary
	Automaton
	AutTable
	Bag
	ConsoleApp
	DateTime
	Exception
	InteractiveShell
	IShell
    OIShell
    LineShell
	Logger
	OptionManager
	orbutils
	Property
    SimpleProperty
	RedBlackTree
	SafeQueue
	CSemaphore
	Singleton
	Sockets
	TextTools
	Throw_if
	XML)

set(headers)
set(sources)
set(incs)

foreach(comp ${components})
	file(GLOB hfiles "sources/${comp}/*.h")
	file(GLOB sfiles "sources/${comp}/*.cpp")
	set(headers ${headers} ${hfiles})
	set(sources ${sources} ${sfiles})
	set(incs ${incs} "sources/${comp}")
endforeach(comp)

#message("headers = ${headers}")
#message("sources = ${sources}")

# Libs (for dynamic libraries)

set(mylibs)

set(libnames
	ACE TAO
	TAO_PortableServer
	TAO_AnyTypeCode
	TAO_CosNaming
	TAO_IORTable
	readline)

# NO_DEFAULT_PATH prevents macos from getting readline from /usr/lib/.
# /opt/local/lib/ is the right path
# NO_DEFAULT_PATH can be used to explicitly set library path search order.

foreach(lib ${libnames})
	set(alib ${lib}-NOTFOUND)
	find_library(alib ${lib} PATHS "/opt/local/lib" "$ENV{ACE_ROOT}/lib" NO_DEFAULT_PATH)
	find_library(alib ${lib}) # will find now if not found in the previous line
	set(mylibs ${mylibs} ${alib})
endforeach(lib)

# message(${mylibs})

# Include directories

include_directories("/opt/local/include" "/opt/local/include/libfmt10"
	"/usr/local/include"
	"$ENV{ACE_ROOT}"
	"$ENV{TAO_ROOT}"
	"$ENV{TAO_ROOT}/orbsvcs"
	${incs}
	"$ENV{COLIBRY}/include")

add_compile_options(-Wno-deprecated-declarations -Wno-deprecated-builtins)

# Libraries

add_library(colibry SHARED ${sources})
add_library(colibry_static STATIC ${sources})

target_link_libraries(colibry ${mylibs} fmt::fmt)

# Installs

install(TARGETS colibry LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/lib)
install(TARGETS colibry_static ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/lib)
install(FILES ${headers} DESTINATION "${PROJECT_SOURCE_DIR}/include/colibry")
