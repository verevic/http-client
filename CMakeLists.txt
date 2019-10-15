# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

include(PrecompiledHeader.cmake)

project ("httpClient")

# Include sub-projects.
add_subdirectory ("httpClient")
