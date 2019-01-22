#!/usr/bin/env bash

set -e

SOURCE="$PWD"
VERSION="$1"
[[ -z "$VERSION" ]] && echo "No version given" && exit -1

echo "Building release $VERSION"
echo "$VERSION" > ./version.txt
export TOM_VERSION="$VERSION"

TARGET="$PWD/release-$VERSION"
mkdir -p "$TARGET"

function buildUbuntu() {
    # ubuntu pakcages
    for name in "$SOURCE"/deployment/ubuntu/*.Dockerfile ; do
        (
            cd "$(dirname "$name")"
            UBUNTU_VERSION="$(basename -s ".Dockerfile" "$name")"
            echo "Building deb for Ubuntu ${UBUNTU_VERSION}"

            DEB_TARGET="$TARGET/$UBUNTU_VERSION"
            mkdir -p "$DEB_TARGET"

            CONTAINER="tom:$UBUNTU_VERSION"
            true docker stop "$CONTAINER"

            docker build --rm -t "$CONTAINER" -f "$name" .
            docker run -d -t -i --rm --name "tom-$UBUNTU_VERSION" -v "$DEB_TARGET:/dist" "$CONTAINER"
            docker exec -e TOM_VERSION "tom-$UBUNTU_VERSION" bash /root/build-ubuntu-deb.sh

            docker stop "$CONTAINER"

            mv "$DEB_TARGET"/*.deb "$TARGET"
        )
    done
}


buildUbuntu