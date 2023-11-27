# tests/CMakeLists.txt

project("TESTS" "C")

# Test executable
add_executable(${PROJECT_NAME} "wrappers_tests.c" "mem_tests.c" "blocks_tests.c" "galloc_tests.c" "gcat_tests.c" "test.c")
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include_tests ${CMAKE_SOURCE_DIR}/include_private ${CMAKE_SOURCE_DIR}/include_public)
target_link_libraries(${PROJECT_NAME} ${project_name})
target_link_libraries(${PROJECT_NAME} wrappers mem blocks galloc GCAT)
target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra)

# Wrappers tests
add_test(NAME TestWrappers COMMAND ./${PROJECT_NAME} wrappers)
add_test(NAME TestWrappers1 COMMAND ./${PROJECT_NAME} wrappers1)
add_test(NAME TestWrappers2 COMMAND ./${PROJECT_NAME} wrappers2)
add_test(NAME TestWrappers3 COMMAND ./${PROJECT_NAME} wrappers3)

# Memory tests
add_test(NAME TestMem COMMAND ./${PROJECT_NAME} mem)
add_test(NAME TestMem1 COMMAND ./${PROJECT_NAME} mem1)
add_test(NAME TestMem2 COMMAND ./${PROJECT_NAME} mem2)

# Blocks test
add_test(NAME TestBlocks COMMAND ./${PROJECT_NAME} blocks)
add_test(NAME TestBlocks01 COMMAND ./${PROJECT_NAME} blocks01)
add_test(NAME TestBlocks02 COMMAND ./${PROJECT_NAME} blocks02)
add_test(NAME TestBlocks03 COMMAND ./${PROJECT_NAME} blocks03)
add_test(NAME TestBlocks04 COMMAND ./${PROJECT_NAME} blocks04)
add_test(NAME TestBlocks05 COMMAND ./${PROJECT_NAME} blocks05)
add_test(NAME TestBlocks06 COMMAND ./${PROJECT_NAME} blocks06)
add_test(NAME TestBlocks07 COMMAND ./${PROJECT_NAME} blocks07)
add_test(NAME TestBlocks08 COMMAND ./${PROJECT_NAME} blocks08)
add_test(NAME TestBlocks09 COMMAND ./${PROJECT_NAME} blocks09)
add_test(NAME TestBlocks10 COMMAND ./${PROJECT_NAME} blocks10)
add_test(NAME TestBlocks11 COMMAND ./${PROJECT_NAME} blocks11)
add_test(NAME TestBlocks12 COMMAND ./${PROJECT_NAME} blocks12)
add_test(NAME TestBlocks13 COMMAND ./${PROJECT_NAME} blocks13)
add_test(NAME TestBlocks14 COMMAND ./${PROJECT_NAME} blocks14)

# Galloc test
add_test(NAME TestGalloc COMMAND ./${PROJECT_NAME} galloc)
add_test(NAME TestGalloc1 COMMAND ./${PROJECT_NAME} galloc1)

# GCAT library test
add_test(NAME TestGcat COMMAND ./${PROJECT_NAME} gcat)
add_test(NAME TestGcat01 COMMAND ./${PROJECT_NAME} gcat01)
add_test(NAME TestGcat02 COMMAND ./${PROJECT_NAME} gcat02)
add_test(NAME TestGcat03 COMMAND ./${PROJECT_NAME} gcat03)
add_test(NAME TestGcat04 COMMAND ./${PROJECT_NAME} gcat04)
add_test(NAME TestGcat05 COMMAND ./${PROJECT_NAME} gcat05)
add_test(NAME TestGcat06 COMMAND ./${PROJECT_NAME} gcat06)
