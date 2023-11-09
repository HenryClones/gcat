# tests/CMakeLists.txt

project("TEST" "C")

# Test executable
add_executable(${PROJECT_NAME} "test.c")
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include_other)
target_link_libraries(${PROJECT_NAME} ${project_name})