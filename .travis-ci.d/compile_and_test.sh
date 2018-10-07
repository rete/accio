#!/bin/bash

mkdir build
cd build
cmake -DCMAKE_CXX_FLAGS="-Werror -Wextra" -DBUILD_TESTING=ON -DCMAKE_INSTALL_PREFIX=$PWD/.. ..
make install
ctest --output-on-failure
