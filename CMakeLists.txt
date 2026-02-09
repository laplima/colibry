#
# (C) 2018-26 by LAPLJ
#
cmake_minimum_required(VERSION 3.25)

project(colibry)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 26)

# find_package(fmt REQUIRED)
find_package(Catch2 3 REQUIRED)    # for testing

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
	list(APPEND headers ${hfiles})
	list(APPEND sources ${sfiles})
	list(APPEND incs "sources/${comp}")
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
	list(APPEND mylibs ${alib})
endforeach(lib)

# message(${mylibs})

# Include directories

include_directories("/opt/local/include"
	"/usr/local/include"
	"$ENV{ACE_ROOT}"
	"$ENV{TAO_ROOT}"
	"$ENV{TAO_ROOT}/orbsvcs"
	"$ENV{COLIBRY}/include")

add_compile_options(
    -Wno-deprecated-declarations -Wno-deprecated-builtins
    -Wall
    # -Wextra
    # -Wshadow
    # -Wconversion
)

# Libraries

function(mktarget pname ptype)
	add_library(${pname} ${ptype})
	add_library(${pname}::${pname} ALIAS ${pname})
	target_sources(${pname} PRIVATE ${sources})
	target_include_directories(${pname}
		PRIVATE ${incs}
		PUBLIC include)
	target_link_libraries(${pname} ${mylibs} Catch2::Catch2WithMain)
	install(TARGETS ${pname} LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/lib)
endfunction()

mktarget(${PROJECT_NAME} SHARED)
mktarget(${PROJECT_NAME}_static STATIC)

# Installs

install(FILES ${headers} DESTINATION "${PROJECT_SOURCE_DIR}/include/colibry")
