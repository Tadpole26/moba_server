PROJECT(network)
CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

INCLUDE_DIRECTORIES(../3rdlibs/libevent/linux)
INCLUDE_DIRECTORIES(../3rdlibs/jemalloc/linux)
INCLUDE_DIRECTORIES(../3rdlibs/zookeeper/linux)
INCLUDE_DIRECTORIES(../platform)
INCLUDE_DIRECTORIES(.)

AUX_SOURCE_DIRECTORY(. DIR_SRCS)

##编译完成后输出路径
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../Bin/Libs/linux)

add_definitions(-DJEMALLOC_NO_DEMANGLE)

SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -Wno-format-security -Wno-deprecated -Wall -rdynamic -std=c++11")
##编译Debug
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -Wall -g -ggdb")
##编译Release
SET(CMAKE_CSS_FLAGS_RELEASE "-O2 -DNDEBUG")
add_library(network ${DIR_SRCS})
