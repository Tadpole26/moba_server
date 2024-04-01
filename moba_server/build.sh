#!/bin/sh
starttime=`date +'%Y-%m-%d %H:%M:%S'`

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
	echo must -D Debug or -R Release!
	exit 1
fi

if [ $config != "Debug" ] && [ $config != "Release" ]
then
	echo must -D Debug or -R Release!
	exit 1
fi

echo $config will build

cd Libs
sh build.sh $config
cd ..

chmod 777 ./build.sh

mkdir -p build/$config/GateServer
mkdir -p build/$config/SysServer
mkdir -p build/$config/GameServer

cd build/$config/GateServer
cmake -DCMAKE_BUILD_TYPE=$config ../../../GateServer
make -j8

cd ../SysServer
cmake -DCMAKE_BUILD_TYPE=$config ../../../SysServer
make -j8

cd ../GameServer
cmake -DCMAKE_BUILD_TYPE=$config ../../../GameServer
make -j8


endtime=`date +'%Y-%m-%d %H:%M:%S'`
start_second=$(date --date="$starttime" +%s);
end_second=$(date --date="$endtime" +%s);
echo "本次编译时间: "$((end_second-start_second))"s"







