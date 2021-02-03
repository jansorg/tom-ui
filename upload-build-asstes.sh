VERSION=0.10.0
TARGET="$PWD/release-$VERSION"

for f in "release-$VERSION"; do
    echo -e "Uploading files in $f..."
    ARGS=""
    for n in "$TARGET"/*.deb "$TARGET"/*.dmg; do
        ARGS="$ARGS -a $n"
    done

    hub release create "v$VERSION" -m "Version $VERSION" $ARGS
done