# tests/CMakeLists.txt

project("TESTS" "C")

# Test executable
add_executable(${PROJECT_NAME} "test.c")
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include_other ${CMAKE_SOURCE_DIR}/include_private ${CMAKE_SOURCE_DIR}/include_public)
target_link_libraries(${PROJECT_NAME} ${project_name})

add_test(NAME TestBlocks COMMAND ${project_name} blocks)
add_test(NAME TestWrappers COMMAND ${project_name} wrappers)
add_test(NAME TestGcat COMMAND ${project_name} gcat)
