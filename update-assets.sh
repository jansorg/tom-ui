#!/usr/bin/env bash
set -e

# export plain svgs for Qt
inkscape -l resources/icons/tom.svg images/Logo.svg
for f in images/icons/*.svg images/icons/*/*.svg; do
    dir="$(dirname $f)"
    dir="resources/${dir#images/}"
    mkdir -p "$dir"
    inkscape -l "$dir/$(basename $f)" "$f"
done

inkscape -l res/AppIcon.svg images/Logo.svg

# export pngs
inkscape -o resources/icons/tom32.png -w 32 -h 32 resources/icons/tom.svg
inkscape -o resources/icons/tom64.png -w 64 -h 64 resources/icons/tom.svg

inkscape -o res/AppIcon32.png -w 32 -h 32 res/AppIcon.svg
inkscape -o res/AppIcon128.png -w 128 -h 128 res/AppIcon.svg

inkscape -o res/AppIcon16.png -w 16 -h 16 res/AppIcon.svg
convert res/AppIcon16.png res/AppIcon.ico && rm res/AppIcon16.png

# create Apple Icons
for s in 16 32 48 128 256;  do
    inkscape -o "res/AppIcon_${s}px.png" -w $s -h "$s" res/AppIcon.svg
done
png2icns res/AppIcon.icns res/AppIcon_*px.png
rm res/AppIcon_*px.png

(cd resources && rm -f resources.qrc && rcc --project -o resources.qrc )