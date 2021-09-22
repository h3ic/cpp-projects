#!/bin/bash

config=$1
if [ -z "$1" ]
then
  config=Debug
fi

mkdir -p "cmake-build-$config"
rm -rf "cmake-build-$config/*"
cmake "-DCMAKE_BUILD_TYPE=$config" -S . -B "cmake-build-$config"
cmake --build "cmake-build-$config"
