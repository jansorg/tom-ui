#!/usr/bin/env bash
set -e

cd $HOME

git clone https://github.com/jansorg/tom-ui
cd tom-ui
[[ -n "$TOM_VERSION" ]] && git checkout "v${TOM_VERSION}"

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE="Release" ..
make -j4

cpack -G DEB
cp *.deb /dist
