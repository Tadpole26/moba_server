PROJECT(GateServer)
CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
INCLUDE_DIRECTORIES( ../Libs/network)
INCLUDE_DIRECTORIES( ../Libs/platform)
INCLUDE_DIRECTORIES( ../Libs/protocol)
INCLUDE_DIRECTORIES( ../Libs/tinyxml)
INCLUDE_DIRECTORIES( ../Libs/serialize)
INCLUDE_DIRECTORIES( ../Libs/3rdlibs/libevent/linux)
INCLUDE_DIRECTORIES( ../Libs/3rdlibs/protobuf/linux)
INCLUDE_DIRECTORIES( . )

LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/../Bin/Libs/linux)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../Bin)

add_definitions(-DJEMALLOC_NO_DEMANGLE)
SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -Wno-format-security -Wno-unused-result -Wno-deprecated-declarations -Wno-deprecated -Wall -rdynamic -std=c++11 -fno-strict-aliasing")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

ADD_EXECUTABLE(GateServer ${DIR_SRCS})
TARGET_LINK_LIBRARIES(GateServer network platform protocol serialize protobuf tinyxml jemalloc event pthread rt ssl event_openssl dl crypto)
