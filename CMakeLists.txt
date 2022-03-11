cmake_minimum_required(VERSION 3.15)
project(MyFileSystem)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add_library(MyFileSystem MyFileSystem.cpp)
add_executable(fstest MyFileSystem.cpp)

