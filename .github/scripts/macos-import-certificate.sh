#!/usr/bin/env bash
# Imports the Developer ID certificate into a temporary keychain and exports
# MACOS_SIGN_IDENTITY and MACOS_KEYCHAIN to $GITHUB_ENV.
# Input: MACOS_SIGN_P12 (base64 encoded .p12) and MACOS_SIGN_PASSWORD.
set -euo pipefail

: "${MACOS_SIGN_P12:?}" "${MACOS_SIGN_PASSWORD:?}" "${RUNNER_TEMP:?}"

keychain="$RUNNER_TEMP/signing.keychain-db"
keychain_password="$(openssl rand -base64 32)"
p12="$RUNNER_TEMP/signing.p12"

printf '%s' "$MACOS_SIGN_P12" | base64 --decode > "$p12"

security create-keychain -p "$keychain_password" "$keychain"
security set-keychain-settings -lut 21600 "$keychain"
security unlock-keychain -p "$keychain_password" "$keychain"
security import "$p12" -k "$keychain" -P "$MACOS_SIGN_PASSWORD" -f pkcs12 -T /usr/bin/codesign
security set-key-partition-list -S apple-tool:,apple:,codesign: -s -k "$keychain_password" "$keychain" >/dev/null
# shellcheck disable=SC2046
security list-keychains -d user -s "$keychain" $(security list-keychains -d user | tr -d '"')
rm -f "$p12"

identity="$(security find-identity -v -p codesigning "$keychain" | grep -m1 -o '"Developer ID Application: [^"]*"' | tr -d '"')"
if [[ -z "$identity" ]]; then
  echo "::error::No Developer ID Application identity found in the certificate"
  exit 1
fi
echo "Signing identity: $identity"

{
  echo "MACOS_SIGN_IDENTITY=$identity"
  echo "MACOS_KEYCHAIN=$keychain"
} >> "$GITHUB_ENV"
