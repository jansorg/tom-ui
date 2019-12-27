#!/usr/bin/env bash

function buildUbuntu() {
    local SOURCE="$1"
    local TARGET="$2"
    local VERSION="$3"
    local CONTAINER="tom-ubuntu"

    # ubuntu packages
    for name in "$SOURCE"/deployment/ubuntu/*.Dockerfile ; do
        (
            cd "$(dirname "$name")"
            UBUNTU_VERSION="$(basename -s ".Dockerfile" "$name")"
            echo "Building deb for Ubuntu ${UBUNTU_VERSION}"

            DEB_TARGET="$TARGET/$UBUNTU_VERSION"
            mkdir -p "$DEB_TARGET"

            docker stop "$CONTAINER" || true

            docker build --rm -t "$CONTAINER" -f "$name" .
            docker run -d -t -i --rm --name "$CONTAINER" -v "$DEB_TARGET:/dist" "$CONTAINER"
            docker exec -e TOM_VERSION "$CONTAINER" bash /root/build-ubuntu-deb.sh

            docker stop "$CONTAINER"

            mv "$DEB_TARGET"/*.deb "$TARGET/tom-ui_${VERSION}-${UBUNTU_VERSION}_amd64.deb"
            rm -rf "$DEB_TARGET"
        )
    done
}

function buildMacOS() {
    local SOURCE="$1"
    local TARGET="$2"
    local VERSION="$3"

    echo "Building DMG for macOS Mojave..."
    ssh mac "cd dev; rm -rf tom-ui; git clone https://github.com/jansorg/tom-ui; cd tom-ui; export TOM_VERSION=$VERSION; bash --login ./deployment/build-mac-dmg.sh" && scp mac:dev/tom-ui/build/Tom.dmg "$TARGET"
    ssh mac "cd dev; rm -rf tom-ui; git clone https://github.com/jansorg/tom-ui; cd tom-ui; export TOM_VERSION=$VERSION; bash --login ./deployment/build-mac-dmg.sh -DENABLE_REPORTS=OFF" && scp mac:dev/tom-ui/build/Tom.dmg "$TARGET/Tom_no-reporting.dmg"
}
