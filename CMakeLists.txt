cmake_minimum_required(VERSION 3.0.0)
project(cpc VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src lib)
add_subdirectory(src)

include(CTest)
enable_testing()
add_subdirectory(test)