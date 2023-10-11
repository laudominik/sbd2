cmake_minimum_required(VERSION 3.0.0)
project(sbd2 VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
add_subdirectory(src)

set(BUILD_TEST true)

if(${BUILD_TEST})
set(TEST_DIR ${CMAKE_SOURCE_DIR}/test)
enable_testing()
add_subdirectory(test)
endif()
