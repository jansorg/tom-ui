#!/bin/bash

. ./release-shared.bash

rm -rf "$PWD/release-dummy"
buildUbuntu "$PWD" "$PWD/release-dummy" "1.0.0"

