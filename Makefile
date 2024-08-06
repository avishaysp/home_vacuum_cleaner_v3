# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic

# Directories
SRCDIR = src
BUILDDIR = build
BINDIR = bin
ALGODIR = $(SRCDIR)/algorithm
ALGO_BUILDDIR = $(BUILDDIR)/algorithm

# Source files
SRCS = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/*/*.cpp)
ALGO_SRCS = $(wildcard $(ALGODIR)/*.cpp)

# Object files
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(filter-out $(ALGO_SRCS), $(SRCS)))

# Algorithm libraries (one .so per algorithm)
ALGO_LIBS = $(patsubst $(ALGODIR)/%.cpp, $(ALGO_BUILDDIR)/%.so, $(ALGO_SRCS))

# Executable name
EXEC = $(BINDIR)/myrobot

# Default target
all: $(EXEC) $(ALGO_LIBS)

# Link object files to create executable
$(EXEC): $(OBJS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile each algorithm source file into its own shared library
$(ALGO_BUILDDIR)/%.so: $(ALGODIR)/%.cpp
	@mkdir -p $(ALGO_BUILDDIR)
	$(CXX) $(CXXFLAGS) -shared -fPIC -o $@ $< -Wl,-undefined,dynamic_lookup

# Compile source files to object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -rf $(BUILDDIR) $(BINDIR)

# Phony targets
.PHONY: all clean
