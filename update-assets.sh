#!/usr/bin/env bash

inkscape -e resources/images/logo32.png -w 32 -h 32 resources/images/logo.svg

inkscape -e res/AppIcon32.png -w 32 -h 32 res/AppIcon.svg
inkscape -e res/AppIcon128.png -w 128 -h 128 res/AppIcon.svg

inkscape -e res/AppIcon16.png -w 16 -h 16 res/AppIcon.svg
convert res/AppIcon16.png res/AppIcon.ico && rm res/AppIcon16.png

# create Apple Icons
for s in 16 32 48 128 256;  do
    inkscape -e res/AppIcon_${s}px.png -w $s -h $s res/AppIcon.svg
done
png2icns res/AppIcon.icns res/AppIcon_*px.png
rm res/AppIcon_*px.png