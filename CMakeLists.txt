cmake_minimum_required(VERSION 3.14)
project(MyRobotProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic -O3")

# Export all symbols for dynamic loading
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/algorithms)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/archive)

# Set custom output directory for GTest libraries
set(GTEST_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/gtest_lib)

# Include directories
include_directories(src)
include_directories(src/algorithm)
include_directories(src/common)
include_directories(src/simulator)
include_directories(googletest/googletest/include)  # Include GTest headers

# Main application source files
set(SIMULATOR_SOURCES
    src/simulator/AlgorithmRegistrar.cpp
    src/simulator/Path.cpp
    src/simulator/args_parser.cpp
    src/simulator/config_reader.cpp
    src/simulator/house.cpp
    src/simulator/io_handling.cpp
    src/simulator/live_simulator.cpp
    src/simulator/simulations_manager.cpp
    src/simulator/simulator.cpp
    src/common/dirt_sensor_impl.cpp
    src/common/wall_sensor_impl.cpp
    src/common/battery_meter_impl.cpp
    src/common/logger.cpp
)

set(APP_MAIN
    src/app_main.cpp
)

# Test source files
set(TEST_SOURCES
    tests/test_main.cpp
    tests/test_simulator.cpp
    src/simulator/Path.cpp
    src/simulator/house.cpp
    src/common/dirt_sensor_impl.cpp
    src/common/wall_sensor_impl.cpp
    src/common/battery_meter_impl.cpp
)

# Algorithm shared libraries - build only, do not link to main executable
add_library(Id_208748665_206476079_RandomSpeedomAlgorithm SHARED
    src/algorithm/random_speedom_algorithm.cpp
    src/algorithm/abstract_speedom_algorithm.cpp
    src/common/AlgorithmRegistrar.h
    src/common/logger.cpp
)

add_library(Id_208748665_206476079_SpeedomAlgorithm SHARED
    src/algorithm/speedom_algorithm.cpp
    src/algorithm/abstract_speedom_algorithm.cpp
    src/common/AlgorithmRegistrar.h
    src/common/logger.cpp
)

set_target_properties(Id_208748665_206476079_RandomSpeedomAlgorithm PROPERTIES PREFIX "")
set_target_properties(Id_208748665_206476079_SpeedomAlgorithm PROPERTIES PREFIX "")

# Main simulator executable
add_executable(myrobot ${SIMULATOR_SOURCES} ${APP_MAIN})
target_link_libraries(myrobot dl pthread)  # Link with dynamic loader and pthread

# Add googletest to the build
add_subdirectory(googletest)

# Set output directory for googletest libraries
set_target_properties(gtest PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${GTEST_OUTPUT_DIRECTORY})
set_target_properties(gtest_main PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${GTEST_OUTPUT_DIRECTORY})

# Set GTest libraries
set(GTEST_LIBRARIES gtest gtest_main)

# Test executable
add_executable(runTests ${TEST_SOURCES} ${SIMULATOR_SOURCES})
target_link_libraries(runTests ${GTEST_LIBRARIES} dl pthread)

# Ensure all targets are built with 'make all_targets'
add_custom_target(all_targets DEPENDS myrobot runTests Id_208748665_206476079_RandomSpeedomAlgorithm Id_208748665_206476079_SpeedomAlgorithm)
