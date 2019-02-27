# Minimum CMake required
cmake_minimum_required(VERSION 3.10)

# Project
project(nforce LANGUAGES CXX VERSION 0.1.0)

# Options
option(NFORCE_BUILD_TESTS "Build tests" ON)
option(NFORCE_BUILD_EXAMPLES "Build examples" ON)

# General Config
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Compilation flags
if (MSVC)
    set(NFORCE_TEST_COMPILE_OPTIONS_DEBUG "/MDd")
    set(NFORCE_TEST_COMPILE_OPTIONS_RELEASE "/MD")
endif()

# Dependencies
add_subdirectory(third_party)

# Lib
set (NFORCE_LIB nforce)

set (NFORCE_INCL
    include/nforce/core/except.h
    include/nforce/core/status.h
    include/nforce/expr.h
    include/nforce/lexer.h
    include/nforce/parser.h
)

set (NFORCE_SRCS
    lib/except.cpp
    lib/lexer.cpp
    lib/parser.cpp
)

add_library(${NFORCE_LIB} ${NFORCE_INCL} ${NFORCE_SRCS})
set_target_properties(${NFORCE_LIB} PROPERTIES FOLDER "lib")
target_compile_features(${NFORCE_LIB} PUBLIC cxx_std_17)
target_include_directories(${NFORCE_LIB} PRIVATE lib PUBLIC include)

# Tests
if (NFORCE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
if (NFORCE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Summary
message(STATUS "Configuration summary")
message(STATUS "-- Project name                 : ${PROJECT_NAME}")
message(STATUS "-- Project version              : ${PROJECT_VERSION}")
