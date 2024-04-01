PROJECT(tinyxml)
CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

AUX_SOURCE_DIRECTORY(. DIR_SRCS)

##编译完成后输出路径
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../Bin/Libs/linux)

add_definitions(-DJEMALLOC_NO_DEMANGLE)

SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -Wno-unuserd_variable -Wno-deprecated -Wall -rdynamic -std=c++11")
##编译Debug
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -Wall -g -ggdb")
##编译Release
SET(CMAKE_CSS_FLAGS_RELEASE "-O2 -DNDEBUG")
add_library(tinyxml ${DIR_SRCS})
