#!/bin/sh

set -e
set -x
set -o pipefail

export MACOSX_DEPLOYMENT_TARGET=10.12

mkdir build && cd build
cmake .. -GXcode
xcodebuild -configuration Release
ctest
