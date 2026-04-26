# COMMON OBJECT LIBRARY

Created by Luiz Lima Jr.

## Compiling & Installing

Compile and install with:

```bash
cd colibry
cmake --preset=dynamic   # cmake -S . -B build --fresh -DBUILD_SHARED_LIBS=ON
cmake --build build
cmake --install build    # uses --prefix .
```

The library will be installed in the directories `${COLIBRY}/lib` and `${COLIBRY}/include` (the current directory `.` corresponds to `${CMAKE_INSTALL_PREFIX}`. To install in another directory, specify CMake's `--prefix` option: `cmake --install build --prefix <install_dir>`.

## Libraries

* `SimTable` &ndash; A basic dictionary string <-> integer (use `Dictionary` instead).
* `Dictionary` &ndash; Implementation of a basic dictionary.
* `Automaton` &ndash; States and transitions defining the behavior of a machine.
* `AutTable` &ndash; Automaton table-based implementation.
* `Bag` &ndash; A bag of data whose items can be collected and put back.
* `ConsoleApp` &ndash; A simple wrapper for console applications.
* `DateTime` &ndash; A simple date-time library (use `std::chrono` for advanced features).
* `Exception` &ndash; Some basic exceptions (rather use `std::stdexcept`).
* `InteractiveShell` &ndash; A command interpreter with completion powers (use `LineShell` instead).
* `IShell` &ndash; A command interpreter with completion (use `LineShell` instead).
* `OIShell` &ndash; A command interpreter using the observer pattern (use `LineShell` instead).
* `LineShell` &ndash; A JSON-based command interpreter with completion for console applications using the observer design pattern.
* `Logger` &ndash; A logger implementation with IO streams.
* `OptionManager` &ndash; A simple handler for command-line options.
* `orbutils` &ndash; A collection of utilities for ACE/TAO CORBA (`ORBManager` and `NameServer`).
* `Property` &ndash; Assign properties to objects.
* `SimpleProperty` &ndash; An even simpler object property implementation.
* `RedBlackTree` &ndash;
* `SafeQueue` &ndash; Thread-safe queue using POSIX.
* `CSemaphore` &ndash;
* `Sockets` &ndash; 
* `TextTools` &ndash; A collection of text and terminal tools.
* `Throw_if` &ndash; An assert-like library that throws custom exceptions upon errors.
* `XML` &ndash;

For usage information and documentation details, see the sources (header/README files).

## Usage

```cmake
cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

project(test)

set(sources main.cpp)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_BUILD_TYPE Debug)

find_package(colibry REQUIRED)

# add_compile_options(-Wall -Wshadow -Wextra -Wconversion)
add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} colibry::colibry)
```
When compiling, provide the path of CMake’s configuration file:
```bash
cmake -S . -B build --fresh -DCMAKE_PREFIX_PATH=<install_dir>/lib/cmake
```

### External library dependencies:

External library dependencies:

* [The GNU Readline Library](https://tiswww.case.edu/php/chet/readline/rltop.html)
* [nlohmann/json](https://github.com/nlohmann/json)
* [ACE/TAO CORBA](https://github.com/DOCGroup/ACE_TAO)

## License

The library is licensed under the MIT License:

Copyright © 2008-2023 Luiz Lima Jr.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
