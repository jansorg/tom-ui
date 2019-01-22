#!/usr/bin/env bash

cd $HOME
go get github.com/jansorg/tom
git clone https://github.com/jansorg/tom-ui
cd tom-ui
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE="Release" ..
make
cpack -G DEB
ls -la
cp *.deb /dist
