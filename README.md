# COMMON OBJECT LIBRARY
_(created from NCOLib - 21/07/2011_
Move from SVN to GIT at 20200912.

(C) 2006-2018 by LAPLJ. All rights reserved.
Luiz Lima Jr.

Compile with:

	$ cd colibry
	$ mkdir build && cd build
	$ cmake ..
	$ make
	$ make install

For usage details, see sources.

2018
	- Included pkg-config file: colibry.pc (see installation instruction in colibry.pc file)
	- Moved from plain makefiles to CMake project (makefiles are now deprecated - removed from SVN - last revision: r33).

External library dependencies:

	- GNU readline
	- ace/tao libs
