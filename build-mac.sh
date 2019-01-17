#!/usr/bin/env bash

go get github.com/jansorg/tom

QTDIR="$HOME/Qt/5.12.0/clang_64"

rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_PREFIX_PATH="$QTDIR"
make
cp $(which tom) tom-ui.app/Contents/MacOS
$QTDIR/bin/macdeployqt tom-ui.app -dmg

