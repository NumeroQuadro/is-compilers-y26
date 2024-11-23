#!/bin/bash

if [ ! -d build ]; then
    mkdir build
fi

cd build

if [ ! -f build.ninja ]; then
    cmake -G Ninja ..
fi

ninja

./bin/yal
