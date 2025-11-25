cmake_minimum_required(VERSION 3.14)
project(avl_tree_tests CXX C)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O2 -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# Include FetchContent module for downloading GoogleTest
include(FetchContent)

# Fetch GoogleTest
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# AVL library source files
set(AVL_SOURCES
    avl.c
)

set(AVL_HEADERS
    avl.h
    avl_impl.h
)

# Create static library
add_library(avl STATIC ${AVL_SOURCES})
target_include_directories(avl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Unit test executable
add_executable(avl_unittest
    avl_unittest.cpp
)

# Link against GoogleTest and AVL library
target_link_libraries(avl_unittest
    PRIVATE
    gtest
    gtest_main
    avl
)

target_include_directories(avl_unittest
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add test to CTest
include(GoogleTest)
gtest_discover_tests(avl_unittest)

# Custom target to run tests
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS avl_unittest
)

# Print configuration info
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
