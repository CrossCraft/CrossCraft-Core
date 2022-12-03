cmake_minimum_required(VERSION 3.17)
project(CrossCraft-Core)

set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE SRC_FILES src/*.c)
file(GLOB_RECURSE INC_FILES include/*.h)

if(NOT VITA)
    set(ZLIB_SRCS ext/zlib/adler32.c ext/zlib/compress.c ext/zlib/crc32.c ext/zlib/deflate.c ext/zlib/gzclose.c ext/zlib/gzlib.c ext/zlib/gzread.c ext/zlib/gzwrite.c ext/zlib/inflate.c ext/zlib/infback.c ext/zlib/inftrees.c ext/zlib/inffast.c ext/zlib/trees.c ext/zlib/uncompr.c ext/zlib/zutil.c)
endif()

set(NBT_SRCS ext/cnbt/buffer.c ext/cnbt/nbt_loading.c ext/cnbt/nbt_parsing.c ext/cnbt/nbt_treeops.c ext/cnbt/nbt_util.c)

add_library(CNBT ${ZLIB_SRCS} ${NBT_SRCS})
target_include_directories(CNBT PUBLIC ext/zlib)

add_library(CCCore ${SRC_FILES} ${INC_FILES})
target_include_directories(CCCore PUBLIC include ext/cnbt/ ext/zlib/)
target_compile_options(CCCore PRIVATE -Wall -Werror -Wextra)
target_link_libraries(CCCore PRIVATE CNBT)