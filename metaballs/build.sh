#!/bin/bash

set -e
set -x

rm -rf build
mkdir build
pushd build

export CONAN_SYSREQUIRES_MODE=enabled
conan install .. -s build_type=Debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
