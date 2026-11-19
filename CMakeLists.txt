cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)
project(cpp_containers)

enable_testing()
add_subdirectory(external/googletest)

add_executable(cpp_containers
    test.cpp
)

target_link_libraries(cpp_containers GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(cpp_containers)
