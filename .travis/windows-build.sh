#!/bin/sh

set -e
set -x


mkdir build && cd build
cmake .. -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DTEAKRA_TEST_ASSETS_DIR="$HOME/assets" -DTEAKRA_RUN_TESTS=OFF
cmake --build . -- -maxcpucount
# ctest -C Release
