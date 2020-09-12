# Parameters.

PROGRAM = testeclp
SOURCES.cpp = \
	../CmdLineParser.cpp	\
	main.cpp
SOURCES.hh =

# Libraries

SLIB = -lreadline

# Derived parameters.

SOURCES = $(SOURCES.cpp)
OBJECTS = $(SOURCES.cpp:%.cpp=%.o)


# C++ Compliler Flags

CXX = clang++
CCFLAGS = -std=c++11
CPPFLAGS =
LDFLAGS =

# Rules for C++.

.SUFFIXES: .cpp .o .h
COMPILE.cpp = $(CXX) $(CCFLAGS) $(CPPFLAGS)
LINK.cpp = $(CXX) $(CCFLAGS) $(LDFLAGS)
.cpp.o:
	$(COMPILE.cc) -c -o $(<:%.cpp=%.o) $<

# Standard targets.

.KEEP_STATE:

all: $(PROGRAM)
objects: $(SOURCES.cpp) $(OBJECTS)
sources: $(SOURCES)
$(PROGRAM): $(SOURCES.cpp) $(OBJECTS)
	$(LINK.cpp) -o $@ $(OBJECTS) $(SLIB)
clean:
	rm -f  $(OBJECTS) core $(PROGRAM)
