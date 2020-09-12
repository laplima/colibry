# laplima 2017
# Parameters

PROGRAM = exctest
SOURCES.cpp = \
	teste_exc.cpp

# Libraries

SLIB = -lcolibry

# Derived parameters.

SOURCES = $(SOURCES.cpp)
OBJECTS = $(SOURCES.cpp:%.cpp=%.o)


# C++ Compiler Flags

CXX = clang++
CCFLAGS = -std=c++14 -I../../../include
CPPFLAGS =
LDFLAGS = -L../../../lib

# Rules for C++.

.SUFFIXES: .cpp .o .h
COMPILE.cpp = $(CXX) $(CCFLAGS) $(CPPFLAGS)
LINK.cpp = $(CXX) $(CCFLAGS) $(LDFLAGS)
.cpp.o:
	$(COMPILE.cpp) -c -o $(<:%.cpp=%.o) $<

# Standard targets.

.KEEP_STATE:

all: $(PROGRAM)
objects: $(SOURCES.cpp) $(OBJECTS)
sources: $(SOURCES)
$(PROGRAM): $(SOURCES.cpp) $(OBJECTS)
	$(LINK.cpp) -o $@ $(OBJECTS) $(SLIB)
clean:
	rm -f  $(OBJECTS) core $(PROGRAM)
