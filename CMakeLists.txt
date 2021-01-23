cmake_minimum_required(VERSION 3.1)
enable_language(CXX)
enable_testing()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
project(EmotionalNES)

add_subdirectory(Programs)
add_subdirectory(Tests)