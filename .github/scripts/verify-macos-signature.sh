#!/usr/bin/env bash
# Fails unless all given binaries have a valid Developer ID signature (ad-hoc or missing signatures fail).
set -uo pipefail

failed=0
for binary in "$@"; do
  echo "--> $binary"
  if ! codesign --verify --strict --verbose=2 "$binary"; then
    echo "::error::$binary: missing or invalid code signature"
    failed=1
    continue
  fi

  details="$(codesign --display --verbose=2 "$binary" 2>&1)"
  echo "$details" | grep -E '^(Authority|TeamIdentifier|Timestamp)='
  if ! echo "$details" | grep -q '^Authority=Developer ID Application:'; then
    echo "::error::$binary: not signed with a Developer ID Application certificate"
    failed=1
  fi
done
exit $failed
