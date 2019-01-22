#!/usr/bin/env bash

SOURCE="$PWD"
VERSION="$1"
[[ -z "$VERSION" ]] && echo "No version given" && exit -1

echo "Building release $VERSION"
echo "$VERSION" > ./version.txt
export TOM_VERSION="$VERSION"

TARGET="$PWD/release-$VERSION"
mkdir -p "$TARGET"

function finish {
    docker stop tom:ubuntu
}

trap finish EXIT

set -e


function buildUbuntu() {
    # ubuntu pakcages
    for name in "$SOURCE"/deployment/ubuntu/*.Dockerfile ; do
        (
            cd "$(dirname "$name")"
            UBUNTU_VERSION="$(basename -s ".Dockerfile" "$name")"
            echo "Building deb for Ubuntu ${UBUNTU_VERSION}"

            DEB_TARGET="$TARGET/$UBUNTU_VERSION"
            mkdir -p "$DEB_TARGET"

            CONTAINER="tom:ubuntu"
            true docker stop "$CONTAINER"

            docker build --rm -t "$CONTAINER" -f "$name" .
            docker run -d -t -i --rm --name "tom-ubuntu" -v "$DEB_TARGET:/dist" "$CONTAINER"
            docker exec -e TOM_VERSION "tom-ubuntu" bash /root/build-ubuntu-deb.sh

            docker stop "tom-ubuntu"

            mv "$DEB_TARGET"/*.deb "$TARGET"
        )
    done
}


buildUbuntu