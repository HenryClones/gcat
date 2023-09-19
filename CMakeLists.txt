cmake_minimum_required(VERSION 3.0)
project("GCAT" "C")
set(CMAKE_C_STANDARD_REQUIRED "C11")

# GCAT library
add_library("gcat" "src/gcat.c" "src/wrappers.c")

# Test executable
add_executable("test" "src/test.c")

include_directories("include")
