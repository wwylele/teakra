#!/bin/sh

set -e
set -x

export CC=gcc-7
export CXX=g++-7

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -- -j"$(nproc)"
ctest
