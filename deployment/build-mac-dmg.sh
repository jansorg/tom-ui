#!/usr/bin/env bash
set -e
DIR="$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd -P)"

[ -z "$APPLE_DEV_PASS" ] && echo "Missing Apple Account Password" && exit 1

FLAGS="$1"
QTDIR="$HOME/Qt/5.15.2/clang_64"
APPLE_DEV_ID="mail@ja-dev.eu"
# 2nd Apple Certificate, made for Tom Time Tracker
APPLE_CERT_ID="F5E7309AFF297C4E9563DD1FFBE3C865D6164EAA"

[[ -n "$TOM_VERSION" ]] && git pull && git checkout "v${TOM_VERSION}"

(
    rm -rf "$HOME/tom/"
    git clone "https://github.com/jansorg/tom.git" "$HOME/tom"
    cd "$HOME/tom/"
    [[ -n "$TOM_VERSION" ]] && git checkout "v${TOM_VERSION}"
    GO111MODULE=on go build -o tom -ldflags "-s -w" .
    codesign -f -s "$APPLE_CERT_ID" tom
)

cd "$DIR/.."

rm -rf build
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH="$QTDIR" -DCMAKE_BUILD_TYPE="Release" "$FLAGS" ..
make -j4

mv tom-ui.app Tom.app
cp "$HOME/tom/tom" Tom.app/Contents/MacOS/tom
install_name_tool -add_rpath "@executable_path/../Frameworks" Tom.app/Contents/MacOS/tom-ui

"$QTDIR/bin/macdeployqt" Tom.app -dmg -always-overwrite -timestamp -codesign="$APPLE_CERT_ID" -sign-for-notarization="$APPLE_CERT_ID"

# notarization, based on https://forum.qt.io/topic/96652/how-to-notarize-qt-application-on-macos/18
# https://stackoverflow.com/questions/5158942/codesign-collisions-between-developer-and-enterprise-distribution
echo "--> Start Notarization process"
response=$(xcrun altool -t osx -f Tom.dmg --primary-bundle-id dev.tomtime.tom --notarize-app -u "${APPLE_DEV_ID}" -p "${APPLE_DEV_PASS}")
requestUUID=$(echo "${response}" | tr ' ' '\n' | tail -1)

set +e
while true; do
  echo "--> Checking notarization status"

  statusCheckResponse=$(xcrun altool --notarization-info "${requestUUID}" -u "${APPLE_DEV_ID}" -p "${APPLE_DEV_PASS}")
  isSuccess=$(echo "${statusCheckResponse}" | grep "success")
  isFailure=$(echo "${statusCheckResponse}" | grep "invalid")

  if [[ "${isSuccess}" != "" ]]; then
      echo "Notarization done!"
      xcrun stapler staple -v Tom.dmg
      echo "Stapler done!"
      exit 0
  fi
  if [[ "${isFailure}" != "" ]]; then
      echo "Notarization failed"
      exit 1
  fi
  echo "Notarization not finished yet, sleep 1m then check again..."
  sleep 60
done