# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic -O3

# Directories
SRCDIR = src
BUILDDIR = build
BINDIR = bin
ALGODIR = $(SRCDIR)/algorithm
ALGO_BUILDDIR = $(BUILDDIR)/algorithm
PREFIX = 208748665_206476079_

# Google Test directories
GTEST_DIR = googletest
GTEST_INCS = -I$(GTEST_DIR)/googletest/include -I$(GTEST_DIR)/googletest
GTEST_SRCS = $(GTEST_DIR)/googletest/src/gtest-all.cc
GTEST_OBJS = $(patsubst $(GTEST_DIR)/googletest/src/%.cc, $(BUILDDIR)/gtest/%.o, $(GTEST_SRCS))

# Source files
SRCS = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/*/*.cpp)
ALGO_SRCS = $(wildcard $(ALGODIR)/*.cpp)

# Main files
APP_MAIN = src/app_main.cpp
TEST_MAIN = tests/test_main.cpp

# Specific algorithm source files
ALGO_SHARED_SRCS_GROUP1 = $(ALGODIR)/random_speedom_algorithm.cpp
ALGO_SHARED_SRCS_GROUP2 = $(ALGODIR)/speedom_algorithm.cpp
ALGO_OBJ_SRCS = $(ALGODIR)/abstract_speedom_algorithm.cpp

# Object files excluding main.cpp for the application executable
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(filter-out $(ALGO_SRCS), $(SRCS))) \
       $(patsubst $(ALGODIR)/%.cpp, $(ALGO_BUILDDIR)/%.o, $(ALGO_OBJ_SRCS))

# Algorithm libraries (one .so per shared algorithm group)
ALGO_LIBS_GROUP1 = $(patsubst $(ALGODIR)/%.cpp, $(ALGO_BUILDDIR)/$(PREFIX)%.so, $(ALGO_SHARED_SRCS_GROUP1))
ALGO_LIBS_GROUP2 = $(patsubst $(ALGODIR)/%.cpp, $(ALGO_BUILDDIR)/$(PREFIX)%.so, $(ALGO_SHARED_SRCS_GROUP2))

# Executable names
EXEC = $(BINDIR)/myrobot
TEST_EXEC = $(BINDIR)/runTests

# Test source files
TEST_SRCS = $(wildcard tests/*.cpp)
TEST_OBJS = $(patsubst tests/%.cpp, $(BUILDDIR)/tests/%.o, $(TEST_SRCS))

# Default target
all: $(PREFIX)random_speedom_algorithm $(PREFIX)speedom_algorithm simulator tests

# Build Google Test object files
$(GTEST_OBJS): $(BUILDDIR)/gtest/%.o: $(GTEST_DIR)/googletest/src/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(GTEST_INCS) -c $< -o $@

# Build algorithm group 1
$(PREFIX)random_speedom_algorithm: $(ALGO_LIBS_GROUP1) $(patsubst $(ALGODIR)/%.cpp, $(ALGO_BUILDDIR)/%.o, $(ALGO_OBJ_SRCS))

# Build algorithm group 2
$(PREFIX)speedom_algorithm: $(ALGO_LIBS_GROUP2) $(patsubst $(ALGODIR)/%.cpp, $(ALGO_BUILDDIR)/%.o, $(ALGO_OBJ_SRCS))

# Build the application executable
simulator: $(EXEC)

# Link object files to create application executable
$(EXEC): $(OBJS) $(BUILDDIR)/app_main.o
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile application main file
$(BUILDDIR)/app_main.o: $(APP_MAIN)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

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

# Compile test source files to object files
$(TEST_OBJS): $(BUILDDIR)/tests/%.o: tests/%.cpp $(GTEST_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(GTEST_INCS) -c $< -o $@

# Build test executable
tests: $(TEST_EXEC)

# Link test object files to create test executable
$(TEST_EXEC): $(TEST_OBJS) $(GTEST_OBJS) $(filter-out $(BUILDDIR)/app_main.o, $(OBJS))
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread

# Compile test main file
$(BUILDDIR)/test_main.o: $(TEST_MAIN)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(GTEST_INCS) -c $< -o $@

# Run tests
.PHONY: test
test: $(TEST_EXEC)

# Clean up build files (add test-related cleanup)
clean:
	rm -rf $(BUILDDIR) $(BINDIR)

# Phony targets
.PHONY: all clean $(PREFIX)random_speedom_algorithm $(PREFIX)speedom_algorithm simulator tests