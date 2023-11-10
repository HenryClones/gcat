# lib/CMakeLists.txt

project("GCAT" "C")

set(SOURCE_FILES "gcat.c")

# GCAT library
include(GenerateExportHeader)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
generate_export_header(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/include_public" PRIVATE "${CMAKE_SOURCE_DIR}/include_private")
# target_link_libraries(gcat "-lpthread")
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(${PROJECT_NAME} PRIVATE galloc)
