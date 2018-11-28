#! /usr/bin/env bash

mkdir -p build_linux
cd build_linux
cmake ..
cmake --build .
cd ..

