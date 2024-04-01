PROJECT(GameServer)
CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
INCLUDE_DIRECTORIES( ../Libs/network)
INCLUDE_DIRECTORIES( ../Libs/platform)
INCLUDE_DIRECTORIES( ../Libs/protocol)
INCLUDE_DIRECTORIES( ../Libs/serialize)
INCLUDE_DIRECTORIES( ../Libs/tinyxml)
INCLUDE_DIRECTORIES( ../Libs/3rdlibs/json/linux)
INCLUDE_DIRECTORIES( ../Libs/3rdlibs/zookeeper/linux)
INCLUDE_DIRECTORIES( ../Libs/3rdlibs/protobuf/linux)
INCLUDE_DIRECTORIES( ../Libs/3rdlibs/curl/linux)
INCLUDE_DIRECTORIES( ../Libs/3rdlibs/mongo/linux/libbson-1.0)
INCLUDE_DIRECTORIES( ../Libs/3rdlibs/mongo/linux/libmongoc-1.0)
INCLUDE_DIRECTORIES( ../Libs/3rdlibs/mongo/linux/bsoncxx/v_noabi)
INCLUDE_DIRECTORIES( ../Libs/3rdlibs/mongo/linux/mongocxx/v_noabi)

LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/../Bin/Libs/linux)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../Bin/)

add_definitions(-Dunix -DJEMALLOC_NO_DEMANGLE)

SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -Wno-format-security -Wno-unused-result -Wno-deprecated-declarations -Wno-deprecated -rdynamic -std=c++11 -fno-strict-aliasing")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

ADD_EXECUTABLE(GameServer ${DIR_SRCS})

TARGET_LINK_LIBRARIES(GameServer network platform tinyxml protocol serialize protobuf zookeeper hashtable jsoncpp bson-1.0 mongoc-1.0 bsoncxx mongocxx jemalloc event pthread rt z event_openssl dl ssl crypto)
