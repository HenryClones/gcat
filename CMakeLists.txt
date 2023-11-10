# CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

set(CMAKE_PROJECT_INCLUDE_BEFORE "${CMAKE_CURRENT_LIST_DIR}/common-project-include.in")

project(${project_name} VERSION ${project_version} LANGUAGES ${project_languages})

# Set the properties
set_property(GLOBAL PROPERTY CMAKE_PROJECT_HOMEPAGE_URL ${project_homepage})
set_property(GLOBAL PROPERTY PROJECT_HOMEPAGE_URL ${project_homepage})
set_property(GLOBAL PROPERTY GCAT_HOMEPAGE_URL ${project_homepage})

# Set required C standards to non-GCC
set(CMAKE_C_STANDARD_REQUIRED "C11")
set(CMAKE_C_COMPILE_FEATURES "Clang")

# Public and private headers used by GCAT
link_directories("include_public")
link_directories("include_private")

# The GCAT wrappers, statically linked
add_subdirectory("libwrap")
# The GCAT wrappers, statically linked
add_subdirectory("libmem")
# The GCAT wrappers, statically linked
add_subdirectory("libblocks")
# The GCAT wrappers, statically linked
add_subdirectory("libgalloc")
# The GCAT library, dynamically linked
add_subdirectory("lib")

# Testing system
include(CTest)
link_directories("include_other")
add_subdirectory("tests")
