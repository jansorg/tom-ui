#!/usr/bin/env bash

set -e

cd "$(dirname "$(readlink -f "$0")")"
. ./release-shared.bash

[[ -z "$GITHUB_USER" ]] && echo "No GITHUB_USER defined" && exit 1
[[ -z "$GITHUB_TOKEN" ]] && echo "No GITHUB_TOKEN defined" && exit 1

SOURCE="$PWD"
VERSION="$1"
[[ -z "$VERSION" ]] && echo "No version given" && exit 1

echo "Building release $VERSION"
echo -n "$VERSION" >./version.txt
git commit -m "Preparing release $VERSION" version.txt || true
git push || true
git tag -f "v$VERSION"
git push -f --tags
export TOM_VERSION="$VERSION"

TARGET="$PWD/release-$VERSION"
mkdir -p "$TARGET"

function finish {
  # docker stop tom-ubuntu || true
  :
}

trap finish EXIT

function uploadAssets() {
  for f in "release-$VERSION"; do
    echo -e "Uploading files in $f..."
    local ARGS=""
    for n in "$TARGET"/*.deb "$TARGET"/*.dmg; do
      ARGS="$ARGS -a $n"
    done

    gh release create "v$VERSION" --title "Version $VERSION" --generate-notes $ARGS
  done
}

function cleanup() {
  local TARGET="$1"
  [[ -d "$TARGET" ]] && rm -rf "$TARGET"
}

buildUbuntu "$SOURCE" "$TARGET" "$VERSION"

buildMacOS "$SOURCE" "$TARGET" "$VERSION"

uploadAssets

cleanup "$TARGET"
