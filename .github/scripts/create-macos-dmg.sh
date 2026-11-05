#!/usr/bin/env bash
# Creates a compressed disk image with the app bundle and a link to /Applications.
# Usage: create-macos-dmg.sh <app bundle> <dmg> [signing identity]
set -euo pipefail

APP="$1"
DMG="$2"
IDENTITY="${3:-}"

staging="$(mktemp -d)"
trap 'rm -rf "$staging"' EXIT

cp -R "$APP" "$staging/"
ln -s /Applications "$staging/Applications"

rm -f "$DMG"
hdiutil create -volname "$(basename "$APP" .app)" -srcfolder "$staging" -fs HFS+ -format UDZO -ov "$DMG"

if [[ -n "$IDENTITY" ]]; then
  codesign --force --timestamp --sign "$IDENTITY" "$DMG"
  codesign --verify --verbose=2 "$DMG"
fi
