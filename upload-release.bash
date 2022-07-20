#!/bin/bash

set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
cd "$DIR"
. ./release-shared.bash

VERSION="$1"
[[ -z "$VERSION" ]] && echo "No version given" && exit 1

uploadAssets "$VERSION" "$DIR/release-$VERSION"
