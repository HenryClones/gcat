# libgalloc/CMakeLists.txt

project("galloc" "C")

set(SOURCE_FILES "galloc.c")

# Wrapper library
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include_private")
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(${PROJECT_NAME} PRIVATE mem)
target_link_libraries(${PROJECT_NAME} PRIVATE blocks)
target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra)
