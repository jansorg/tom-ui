#!/usr/bin/env bash
# Signs a deployed app bundle from the inside out with the hardened runtime, as required for notarization.
# Binaries in Contents/MacOS other than the main executable (the bundled tom binaries) keep their own signatures.
# Usage: sign-macos-app.sh <app bundle> <identity>, identity "-" signs ad-hoc
set -euo pipefail

APP="$1"
IDENTITY="$2"

if [[ "$IDENTITY" == "-" ]]; then
  # ad-hoc signatures support neither timestamps nor notarization
  SIGN_ARGS=(--force --sign -)
else
  SIGN_ARGS=(--force --timestamp --options runtime --sign "$IDENTITY")
fi

sign() {
  codesign "${SIGN_ARGS[@]}" "$@"
}

# Qt WebEngine's helper process needs the entitlements shipped with Qt,
# see https://doc.qt.io/qt-6/qtwebengine-deploying.html
while IFS= read -r -d '' helper; do
  echo "--> $helper"
  sign --entitlements "$helper/Contents/Resources/QtWebEngineProcess.entitlements" "$helper"
done < <(find "$APP/Contents/Frameworks" -type d -name 'QtWebEngineProcess.app' -print0)

while IFS= read -r -d '' lib; do
  sign "$lib"
done < <(find "$APP/Contents" -type f -name '*.dylib' -print0)

for framework in "$APP"/Contents/Frameworks/*.framework; do
  [[ -d "$framework" ]] && sign "$framework"
done

echo "--> $APP"
sign "$APP"

codesign --verify --deep --strict --verbose=2 "$APP"
