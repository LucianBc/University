cmake_minimum_required(VERSION 3.6)
project(C2)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp DirectedGraph.cpp DirectedGraph.h)
add_executable(C2 ${SOURCE_FILES})