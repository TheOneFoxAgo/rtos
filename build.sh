#!/bin/bash

if [[ "$1" == "on" ]]; then
    CMAKE_OPT="-DENABLE_DEBUG_LOG=ON"
else
    CMAKE_OPT="-DENABLE_DEBUG_LOG=OFF"
fi

cmake -B build $CMAKE_OPT -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build -- -j$(nproc)
