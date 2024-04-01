#!/bin/sh

config=$1

if [ $config = "-D" ]
then
	config="Debug"
fi

if [ $config = "-R" ]
then
	config="Release"
fi

if [ -z $config ]
then
	echo must -D as debug or -R as release!
	exit 1
fi

mkdir -p build/$config/network
mkdir -p build/$config/platform
mkdir -p build/$config/protocol
mkdir -p build/$config/serialize
mkdir -p build/$config/tinyxml

chmod 777 ./build.sh

cd build/$config/network
cmake -DCMAKE_BUILD_TYPE=$config ../../../network/
make -j8

cd ../platform
cmake -DCMAKE_BUILD_TYPE=$config ../../../platform
make -j8

cd ../protocol
cmake -DCMAKE_BUILD_TYPE=$config ../../../protocol
make -j8

cd ../serialize
cmake -DCMAKE_BUILD_TYPE=$config ../../../serialize
make -j8

cd ../tinyxml
cmake -DCMAKE_BUILD_TYPE=$config ../../../tinyxml
make -j8
