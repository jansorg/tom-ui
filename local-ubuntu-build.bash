#!/bin/bash

. ./release-shared.bash

rm -rf "$PWD/release-dummy"
mkdir "$PWD/release-dummy"
buildUbuntu "$PWD" "$PWD/release-dummy" ""

