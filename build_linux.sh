#!/bin/sh
build_type=Release
if [ "$1" = "debug" ]
then
   build_type=Debug
fi
echo "Building chipbox for linux, type $build_type"
rm -rf build/
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=$build_type ../src
cmake --build .

cd ..
rm -rf dist/
mkdir -p dist/bin dist/lib
cp build/bin/chipbox_sdl build/bin/testcore dist/bin
cp build/lib/libchipbox*.a dist/lib
cp README.md LICENSE TODO.md dist/
