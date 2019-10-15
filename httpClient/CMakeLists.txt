# CMakeList.txt : CMake project for httpClient, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

add_subdirectory(src)
add_subdirectory(test)

add_executable (${PROJECT_NAME} ${SOURCES} "httpClient.cpp")
target_include_directories(${PROJECT_NAME} PUBLIC include src)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib)
