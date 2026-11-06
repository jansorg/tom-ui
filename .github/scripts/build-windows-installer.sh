#!/usr/bin/env bash
# Builds the Inno Setup installer for a deployed application directory.
# Usage: build-windows-installer.sh <app dir> <setup.iss> <output dir>
# Must be run in the source directory, which provides LICENSE.txt.
set -euo pipefail

APP_DIR="$1"
ISS_FILE="$2"
OUT_DIR="$3"

mkdir -p "$OUT_DIR"
"/c/Program Files (x86)/Inno Setup 6/ISCC.exe" //Q \
  "//DLICENSE_FILE=$(cygpath -w "$(realpath LICENSE.txt)")" \
  "//DPREFIX=$(cygpath -w "$(realpath "$APP_DIR")")" \
  "//O$(cygpath -w "$(realpath "$OUT_DIR")")" \
  "$(cygpath -w "$ISS_FILE")"
ls -l "$OUT_DIR"
