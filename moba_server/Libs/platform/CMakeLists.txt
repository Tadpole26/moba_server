PROJECT(platform)
CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

INCLUDE_DIRECTORIES(../3rdlibs/boost/linux)
INCLUDE_DIRECTORIES(../3rdlibs/mongo/linux/libbson-1.0)
INCLUDE_DIRECTORIES(../3rdlibs/mongo/linux/libmongoc-1.0)
INCLUDE_DIRECTORIES(../3rdlibs/mongo/linux/mongocxx/v_noabi)
INCLUDE_DIRECTORIES(../3rdlibs/mongo/linux/bsoncxx/v_noabi)
INCLUDE_DIRECTORIES(../3rdlibs/mysql/linux)
INCLUDE_DIRECTORIES(../3rdlibs/mysqlpp/linux)
INCLUDE_DIRECTORIES(../3rdlibs/curl/linux)
INCLUDE_DIRECTORIES(../3rdlibs/jemalloc/linux)
INCLUDE_DIRECTORIES(../network)
INCLUDE_DIRECTORIES(../tinyxml)

AUX_SOURCE_DIRECTORY(. DIR_SRCS)

##编译完成后输出路径
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../Bin/Libs/linux)

add_definitions(-DJEMALLOC_NO_DEMANGLE -Dunix)

SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -Wno-format-security -Wno-unused-result -Wno-deprecated-declarations -Wno-deprecated -Wall -rdynamic -std=c++11 -fPIC")
##编译Debug
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -Wall -g -ggdb")
##编译Release
SET(CMAKE_CSS_FLAGS_RELEASE "-O2 -DNDEBUG")
add_library(platform ${DIR_SRCS})

