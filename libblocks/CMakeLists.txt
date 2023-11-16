# libblocks/CMakeLists.txt

project("blocks" "C")

set(SOURCE_FILES "block_properties.c" "block_array.c")

# Wrapper library
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include_private")
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
