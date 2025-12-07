#!/bin/bash

cd ./../third-party/visualization-3d/server
mkdir -p build
cd build

if [ ! -f "server" ]; then
    cmake ..
    make -j
fi

./server
