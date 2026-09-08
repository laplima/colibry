#
# (C) 2018-26 by LAPLJ
#

# presets
cmake_minimum_required(VERSION 3.31)

# include(GNUInstallDirs)
# include(CMakePackageConfigHelpers)

project(colibry
	VERSION 2.2.0
	DESCRIPTION "Common Object Library")

set(libname ${PROJECT_NAME})

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

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
	ipc
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
	list(APPEND incs "${PROJECT_SOURCE_DIR}/sources/${comp}")
endforeach(comp)

# external libs

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

# Include directories

include_directories("/opt/local/include"
	"/usr/local/include"
	"$ENV{ACE_ROOT}"
	"$ENV{TAO_ROOT}"
	"$ENV{TAO_ROOT}/orbsvcs"
	# "$ENV{COLIBRY}/include"
)

add_compile_options(
    -Wno-deprecated-declarations -Wno-deprecated-builtins
    -Wall
    # -Wextra
    # -Wshadow
    # -Wconversion
)

# set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}")
# set(CMAKE_INSTALL_PREFIX "/tmp/co")

# Libraries

# Library (both static and dynamic)
# STATIC / SHARED not specified (depends on BUILD_SHARED_LIBS=OFF/ON -- default = OFF)

add_library(${libname})
add_library(${libname}::${libname} ALIAS ${libname})
target_sources(${libname} PRIVATE ${sources})
target_include_directories(${libname}
	PRIVATE ${incs}
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sources/${libname}>
	$<INSTALL_INTERFACE:include/${libname}>)
set_target_properties(${libname} PROPERTIES PUBLIC_HEADER "${headers}")
target_link_libraries(${libname} ${mylibs})
	# Catch2::Catch2WithMain)

install(TARGETS ${libname}
		EXPORT ${libname}-targets
		LIBRARY
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${libname}
		INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT ${libname}-targets
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${libname}
	NAMESPACE ${libname}::
	FILE ${libname}-targets.cmake) # must have .cmake extension

# Generate and install the config file
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${libname}-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${libname}-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${libname})

# (Optionally generate a version file)
write_basic_package_version_file(
    ${libname}-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion)

# install files
install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/${libname}-config.cmake
	${CMAKE_CURRENT_BINARY_DIR}/${libname}-config-version.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${libname})
