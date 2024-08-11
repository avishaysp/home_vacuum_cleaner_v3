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
PREFIX = 208748665_206476079_

# Source files
SRCS = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/*/*.cpp)
ALGO_SRCS = $(wildcard $(ALGODIR)/*.cpp)

# Specific algorithm source files
ALGO_SHARED_SRCS_GROUP1 = $(ALGODIR)/random_speedom_algorithm.cpp
ALGO_SHARED_SRCS_GROUP2 = $(ALGODIR)/speedom_algorithm.cpp
ALGO_OBJ_SRCS = $(ALGODIR)/abstract_speedom_algorithm.cpp

# Object files
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(filter-out $(ALGO_SRCS), $(SRCS))) \
       $(patsubst $(ALGODIR)/%.cpp, $(ALGO_BUILDDIR)/%.o, $(ALGO_OBJ_SRCS))

# Algorithm libraries (one .so per shared algorithm group)
ALGO_LIBS_GROUP1 = $(patsubst $(ALGODIR)/%.cpp, $(ALGO_BUILDDIR)/$(PREFIX)%.so, $(ALGO_SHARED_SRCS_GROUP1))
ALGO_LIBS_GROUP2 = $(patsubst $(ALGODIR)/%.cpp, $(ALGO_BUILDDIR)/$(PREFIX)%.so, $(ALGO_SHARED_SRCS_GROUP2))

# Executable name
EXEC = $(BINDIR)/myrobot

# Default target
all: $(PREFIX)random_speedom_algorithm $(PREFIX)speedom_algorithm simulator

# Build algorithm group 1
$(PREFIX)random_speedom_algorithm: $(ALGO_LIBS_GROUP1) $(patsubst $(ALGODIR)/%.cpp, $(ALGO_BUILDDIR)/%.o, $(ALGO_OBJ_SRCS))

# Build algorithm group 2
$(PREFIX)speedom_algorithm: $(ALGO_LIBS_GROUP2) $(patsubst $(ALGODIR)/%.cpp, $(ALGO_BUILDDIR)/%.o, $(ALGO_OBJ_SRCS))

# Build the rest of the project
simulator: $(EXEC)

# Link object files to create executable
$(EXEC): $(OBJS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile each shared algorithm source file into its own shared library (group 1)
$(ALGO_BUILDDIR)/$(PREFIX)%.so: $(ALGODIR)/%.cpp
	@mkdir -p $(ALGO_BUILDDIR)
	$(CXX) $(CXXFLAGS) -shared -fPIC -o $@ $< -Wl,-undefined,dynamic_lookup

# Compile each shared algorithm source file into its own shared library (group 2)
$(ALGO_BUILDDIR)/$(PREFIX)%.so: $(ALGODIR)/%.cpp
	@mkdir -p $(ALGO_BUILDDIR)
	$(CXX) $(CXXFLAGS) -shared -fPIC -o $@ $< -Wl,-undefined,dynamic_lookup

# Compile specific algorithm source files into object files
$(ALGO_BUILDDIR)/%.o: $(ALGODIR)/%.cpp
	@mkdir -p $(ALGO_BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile source files to object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -rf $(BUILDDIR) $(BINDIR)

# Phony targets
.PHONY: all clean $(PREFIX)random_speedom_algorithm $(PREFIX)speedom_algorithm simulator