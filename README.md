# COMMON OBJECT LIBRARY

## Compiling & Installing

Compile with:

	$ cd colibry
	$ mkdir build && cd build
	$ cmake ..
	$ make
	$ make install

`make install` will install by default in the directory `${PROJECT_SOURCE_DIR}/lib` and 1 `${PROJECT_SOURCE_DIR}/include` directories (thus, there is no need to run the command as root).

## Libraries

* `SimTable` &ndash; a basic dictionary string <-> integer (use `Dictionary` instead);
* `Dictionary` &ndash; a basic dictionary;
* `Automaton` &ndash; states and transitions defining the behavior of a machine;
* `AutTable` &ndash; automaton table-based implementation;
* `Bag` &ndash; a bag of data;
* `ConsoleApp` &ndash; simple wrapper for console applications;
* `DateTime` &ndash; simple date-time library;
* `Exception` &ndash; basic exceptions;
* `InteractiveShell` &ndash; a command interpreter with completion (use `LineShell` instead);
* `IShell` &ndash; a command interpreter with completion (use `LineShell` instead);
* `OIShell` &ndash; a command interpreter using the observer pattern (use `LineShell` instead);
* `LineShell` &ndash; a json-based command interpreter with completion;
* `Logger` &ndash; a logger implementation with IO streams;
* `OptionManager` &ndash; handles command-line options;
* `orbutils &ndash; several utilities for ACE/TAO CORBA (`ORBManager` and `NameServer`);
* `Property` &ndash; simple object properties;
* `SimpleProperty` &ndash; even simpler object properties;
* `RedBlackTree` &ndash;
* `SafeQueue` &ndash; thread-safe queue using POSIX
* `CSemaphore` &ndash;
* `Sockets` &ndash; 
* `TextTools` &ndash;
* `Throw_if` &ndash; 
* `XML` &ndash;

For usage details and documentation, see sources.

2018
	- Included pkg-config file: colibry.pc (see installation instruction in colibry.pc file)
	- Moved from plain makefiles to CMake project (makefiles are now deprecated - removed from SVN - last revision: r33).

External library dependencies:

- [GNU readline](https://tiswww.case.edu/php/chet/readline/rltop.html)
- [ACE/TAO libs](https://download.dre.vanderbilt.edu)
- [nlohmann/json](https://github.com/nlohmann/json)

## License

The library is licensed under the MIT License:

Copyright © 2008-2023 Luiz Lima Jr.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
