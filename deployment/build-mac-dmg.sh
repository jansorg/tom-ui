#!/usr/bin/env bash
FLAGS="$1"

git pull
[[ -n "$TOM_VERSION" ]] && git checkout "v${TOM_VERSION}"

go get github.com/jansorg/tom
(cd $HOME/go/src/github.com/jansorg/tom/ && git pull)

QTDIR="$HOME/Qt/5.12.0/clang_64"

rm -rf build
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH="$QTDIR" -DCMAKE_BUILD_TYPE="Release" "$FLAGS" ..
make -j4

mv tom-ui.app Tom.app
go build -o Tom.app/Contents/MacOS/tom -ldflags "-s -w" github.com/jansorg/tom
install_name_tool -add_rpath "@executable_path/../Frameworks" Tom.app/Contents/MacOS/tom-ui
"$QTDIR/bin/macdeployqt" Tom.app -dmg
