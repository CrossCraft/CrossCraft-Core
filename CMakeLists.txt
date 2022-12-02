cmake_minimum_required(VERSION 3.17)
project(CrossCraft-Core)

set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE SRC_FILES src/*.c)
file(GLOB_RECURSE INC_FILES include/*.h)

add_library(CCCore ${SRC_FILES} ${INC_FILES})
target_include_directories(CCCore PUBLIC include)
target_compile_options(CCCore PRIVATE -Wall -Werror -Wextra)