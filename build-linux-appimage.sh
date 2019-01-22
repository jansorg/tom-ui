#!/usr/bin/env bash

go get github.com/jansorg/tom

#QTDIR="$HOME/Qt/5.12.0/clang_64"

rm -rf build
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE="Release" ..
make

go build -o tom -ldflags "-s -w" github.com/jansorg/tom
linuxdeployqt tom-ui
