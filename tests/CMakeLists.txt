# tests/CMakeLists.txt

project("TESTS" "C")

# Test executable
add_executable(${PROJECT_NAME} "test.c")
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include_other ${CMAKE_SOURCE_DIR}/include_private ${CMAKE_SOURCE_DIR}/include_public)
target_link_libraries(${PROJECT_NAME} ${project_name})
target_link_libraries(${PROJECT_NAME} wrappers)

add_test(NAME TestBlocks COMMAND ${PROJECT_NAME} blocks)
add_test(NAME TestWrappers COMMAND ${PROJECT_NAME} wrappers)
add_test(NAME TestGcat COMMAND ${PROJECT_NAME} gcat)
