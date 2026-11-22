cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)
project(cpp_containers)

option(ENABLE_ASAN "Enable AddressSanitizer" ON)

if(ENABLE_ASAN)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer -g)
    add_link_options(-fsanitize=address)
endif()

enable_testing()
add_subdirectory(external/googletest)

add_executable(cpp_containers
    test.cpp
)

target_link_libraries(cpp_containers GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(cpp_containers)
