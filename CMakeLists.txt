# CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

set(CMAKE_PROJECT_INCLUDE_BEFORE "${CMAKE_CURRENT_LIST_DIR}/common-project-include.in")

project(${project_name} VERSION ${project_version} LANGUAGES ${project_languages})
enable_testing()

set_property(GLOBAL PROPERTY CMAKE_PROJECT_HOMEPAGE_URL ${project_homepage})
set_property(GLOBAL PROPERTY PROJECT_HOMEPAGE_URL ${project_homepage})
set_property(GLOBAL PROPERTY GCAT_HOMEPAGE_URL ${project_homepage})

set(CMAKE_C_STANDARD_REQUIRED "C11")
set(CMAKE_C_COMPILE_FEATURES "Clang")

link_directories("include_public")
link_directories("include_private")

add_subdirectory("lib")
add_subdirectory("libwrap")

link_directories("include_other")
add_subdirectory("tests")
