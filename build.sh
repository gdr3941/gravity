#!/bin/bash

# Project build script using Cmake and Conan

[ -d "build" ] || mkdir build
cd build
conan install --build=missing .. &&
    cmake .. &&
    cmake --build . &&
    echo "== Successfully built project. See executables in build/bin =="
