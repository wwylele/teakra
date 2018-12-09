#!/bin/sh

set -e
set -x
set -o pipefail

export MACOSX_DEPLOYMENT_TARGET=10.14

mkdir build && cd build
cmake .. -GXcode -DCMAKE_BUILD_TYPE=Release
xcodebuild -configuration Release
ctest
