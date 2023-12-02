# Makefile for generating debug and optimized versions

# Compiler
CXX = g++
# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++11

# Debug flags (add debugging symbols)
DEBUG_FLAGS = -g

# Optimized flags (enable optimization)
OPTIMIZE_FLAGS = -O3

# Source files
SRC = project.cpp
#HEADER = header.h

# Output binaries
DEBUG_BIN = debug_binary
OPTIMIZE_BIN = optimize_binary

# Default target
all: debug optimize

# Debug target
debug: $(SRC) $(HEADER)
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) $(SRC) -o $(DEBUG_BIN)

# Optimized target
optimize: $(SRC) $(HEADER)
	$(CXX) $(CXXFLAGS) $(OPTIMIZE_FLAGS) $(SRC) -o $(OPTIMIZE_BIN)

# Clean target
clean:
	rm -f $(DEBUG_BIN) $(OPTIMIZE_BIN)

.PHONY: all debug optimize clean