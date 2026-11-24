cmake_minimum_required(VERSION 3.15)

# enable leak sanitizer for apple clang
if(APPLE AND NOT CMAKE_CXX_COMPILER AND EXISTS /opt/homebrew/opt/llvm/bin/clang++)
    set(CMAKE_C_COMPILER   /opt/homebrew/opt/llvm/bin/clang)
    set(CMAKE_CXX_COMPILER /opt/homebrew/opt/llvm/bin/clang++)
endif()

project(cpp_containers)
set(CMAKE_CXX_STANDARD 20)

option(ENABLE_ASAN "Enable AddressSanitizer" ON)
if(ENABLE_ASAN)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer -g)
    add_link_options(-fsanitize=address)
endif()

enable_testing()
add_subdirectory(external/googletest)

add_executable(cpp_containers 
    src/vector_test.cpp
)
target_include_directories(cpp_containers PRIVATE include)
target_link_libraries(cpp_containers GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(cpp_containers
    PROPERTIES ENVIRONMENT "ASAN_OPTIONS=detect_leaks=1")