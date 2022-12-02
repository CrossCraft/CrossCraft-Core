cmake_minimum_required(VERSION 3.17)
project(CrossCraft-Core)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
file(GLOB_RECURSE INC_FILES include/*.hpp)

add_library(CrossCraft-Core ${SRC_FILES} ${INC_FILES})