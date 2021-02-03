#!/bin/bash

. ./release-shared.bash

rm -rf "$PWD/release-dummy"
mkdir -p "$PWD/release-dummy"
buildMacOS "$PWD" "$PWD/release-dummy" ""

