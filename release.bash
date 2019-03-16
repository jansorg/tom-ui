#!/usr/bin/env bash

set -e

cd "$(dirname "$(readlink -f "$0")")"
. ./release-shared.bash

[[ -z "$GITHUB_USER" ]] && echo "No GITHUB_USER defined" && exit -1
[[ -z "$GITHUB_TOKEN" ]] && echo "No GITHUB_TOKEN defined" && exit -1

SOURCE="$PWD"
VERSION="$1"
[[ -z "$VERSION" ]] && echo "No version given" && exit -1

echo "Building release $VERSION"
echo -n "$VERSION" > ./version.txt
git commit -m "Prepearing release of $VERSION" version.txt
git push
git tag "v$VERSION"
git push --tags
export TOM_VERSION="$VERSION"

TARGET="$PWD/release-$VERSION"
mkdir -p "$TARGET"

function finish {
    docker stop tom-ubuntu || true
}

trap finish EXIT

function uploadAssets() {
    for f in "release-$VERSION"; do
        echo -e "Uploading files in $f..."
        ARGS=""
        for n in $TARGET/*.deb $TARGET/*.dmg; do
            ARGS="$ARGS -a $n"
        done

        hub release create "v$VERSION" -m "Version $VERSION" $ARGS
    done
}

function cleanup() {
    rm -rf "$TARGET"
}

buildUbuntu "$SOURCE" "$TARGET" "$VERSION"

buildMacOS "$SOURCE" "$TARGET" "$VERSION"

uploadAssets

cleanup