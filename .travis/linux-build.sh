#!/bin/sh

set -e
set -x

export CC=gcc-7
export CXX=g++-7

mkdir build && cd build
cmake ..
make
ctest
