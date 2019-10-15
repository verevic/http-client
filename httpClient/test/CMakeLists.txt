# CMakeList.txt (tests)
#
cmake_minimum_required (VERSION 3.8)
find_package(GTest REQUIRED)
# I suspect, there should be a way to make gtest not use threads
find_package (Threads REQUIRED)

# add_executable (${PROJECT_NAME}_gtest "URLTest.cpp" "httpClientTest.cpp" "allTests.cpp")
add_executable (${PROJECT_NAME}_gtest "URLTest.cpp" "httpClientTest.cpp")
target_include_directories(${PROJECT_NAME}_gtest PUBLIC ../include ../src ${GTEST_INCLUDE_DIR})
# target_link_libraries(${PROJECT_NAME}_gtest ${PROJECT_NAME}_lib ${GTEST_LIBRARIES})
target_link_libraries(${PROJECT_NAME}_gtest ${PROJECT_NAME}_lib ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

# TODO: Add tests and install targets if needed.
enable_testing ()

# add_test(testURL ${PROJECT_NAME}_gtest)
gtest_add_tests(TARGET ${PROJECT_NAME}_gtest)
