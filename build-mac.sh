#!/usr/bin/env bash

go get github.com/jansorg/tom

QTDIR="$HOME/Qt/5.12.0/clang_64"

rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_PREFIX_PATH="$QTDIR"
make

mv tom-ui.app Tom.app
go build -o Tom.app/Contents/MacOS/tom -ldflags "-s -w" github.com/jansorg/tom
"$QTDIR/bin/macdeployqt" Tom.app -dmg

