#!/bin/sh

set -e
set -x


mkdir build && cd build
cmake .. -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release
cmake --build . -- -maxcpucount
ctest
