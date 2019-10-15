# CMakeList.txt : CMake project for httpClient, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(SOURCES "pch.cpp" "URL.cpp" "tcp_client.cpp" "http_client.cpp" "http_request.cpp")
add_library(${PROJECT_NAME}_lib STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME}_lib PUBLIC ../include)

add_precompiled_header (${PROJECT_NAME}_lib "pch.h" SOURCE_CXX "pch.cpp" FORCEINCLUDE)

