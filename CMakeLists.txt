cmake_minimum_required(VERSION 3.0)
project("GCAT" "C")
set(CMAKE_C_STANDARD_REQUIRED "C11")

# GCAT library
add_library(gcat SHARED "src/gcat.c" "src/wrappers.c")
target_link_libraries(gcat "-lpthread")
target_link_libraries(gcat "-Llibgcat.a>")

# Test executable
add_executable(test "src/test.c")
target_link_libraries(test gcat)

include_directories("include")
