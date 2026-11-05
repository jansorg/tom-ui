#!/usr/bin/env bash
#
# Signs the Windows release of tom-ui with the code signing certificate on a YubiKey
# and adds the signed installer to the draft GitHub release.
#
# Usage: deployment/windows/sign-windows-release.sh <version>, e.g. 0.13.0
#
# Steps:
#   1. waits for the build.yml run of tag v<version>, which creates the draft release
#   2. downloads the unsigned application ("windows-app" artifact) and signs tom-ui.exe
#   3. uploads the signed tom-ui.exe to the draft release and starts windows-installer.yml,
#      which builds the installer with it
#   4. signs the installer and uploads it to the draft release
#
# The private key never leaves the YubiKey. Publishing the draft release is left to you.
#
# Requirements: gh (logged in), jsign, osslsigncode, the YubiKey PKCS#11 module (libykcs11, yubico-piv-tool).
#
# Configuration via environment variables:
#   TOM_SIGN_ALIAS  certificate alias on the YubiKey (default: slot 9c, "X.509 Certificate for Digital Signature")
#   TOM_SIGN_TSA    RFC 3161 timestamp server (default: http://timestamp.digicert.com)
#   TOM_SIGN_CA     CA bundle to verify the signatures (default: /etc/ssl/certs/ca-certificates.crt)
#   TOM_SIGN_PIN    YubiKey PIV PIN, prompted if not set

set -euo pipefail

REPO="jansorg/tom-ui"
ALIAS="${TOM_SIGN_ALIAS:-X.509 Certificate for Digital Signature}"
TSA="${TOM_SIGN_TSA:-http://timestamp.digicert.com}"
CA_BUNDLE="${TOM_SIGN_CA:-/etc/ssl/certs/ca-certificates.crt}"

VERSION="${1:-}"
[[ -z "$VERSION" ]] && echo "Usage: $0 <version>" >&2 && exit 1
TAG="v${VERSION#v}"

for cmd in gh jsign osslsigncode; do
  command -v "$cmd" >/dev/null || { echo "Missing required command: $cmd" >&2; exit 1; }
done

WORK="$(mktemp -d)"
TEMP_ASSET_UPLOADED=false

cleanup() {
  if [[ "$TEMP_ASSET_UPLOADED" == true ]]; then
    echo "--> Removing temporary tom-ui.exe from the draft release"
    gh release delete-asset "$TAG" tom-ui.exe --repo "$REPO" --yes || true
  fi
  rm -rf "$WORK"
}
trap cleanup EXIT

if [[ -z "${TOM_SIGN_PIN:-}" ]]; then
  read -rsp "YubiKey PIV PIN: " TOM_SIGN_PIN
  echo
fi

sign() {
  local file="$1"
  echo "--> Signing $(basename "$file") (touch the YubiKey if it blinks)"
  TOM_SIGN_PIN="$TOM_SIGN_PIN" jsign --storetype YUBIKEY --storepass env:TOM_SIGN_PIN --alias "$ALIAS" \
    --tsaurl "$TSA" --tsmode RFC3161 \
    --name "Tom" --url "https://github.com/$REPO" \
    "$file"
  osslsigncode verify -in "$file" -CAfile "$CA_BUNDLE" >/dev/null
  echo "    signature verified"
}

echo "--> Waiting for the build of $TAG"
BUILD_RUN="$(gh run list --repo "$REPO" --workflow build.yml --branch "$TAG" --event push --limit 1 \
  --json databaseId -q '.[0].databaseId')"
[[ -z "$BUILD_RUN" ]] && echo "No build.yml run found for $TAG. Was the tag pushed?" >&2 && exit 1
gh run watch "$BUILD_RUN" --repo "$REPO" --exit-status --interval 30 >/dev/null

if [[ "$(gh release view "$TAG" --repo "$REPO" --json isDraft -q .isDraft)" != "true" ]]; then
  echo "Release $TAG is not a draft, refusing to modify it" >&2
  exit 1
fi

echo "--> Downloading the unsigned application of run $BUILD_RUN"
gh run download "$BUILD_RUN" --repo "$REPO" --name windows-app --dir "$WORK/app"
sign "$WORK/app/bin/tom-ui.exe"

echo "--> Uploading the signed tom-ui.exe to the draft release"
gh release upload "$TAG" "$WORK/app/bin/tom-ui.exe" --repo "$REPO" --clobber
TEMP_ASSET_UPLOADED=true

echo "--> Building the installer"
STARTED="$(date -u +%Y-%m-%dT%H:%M:%SZ)"
gh workflow run windows-installer.yml --repo "$REPO" --ref "$TAG" -f tag="$TAG" -f build-run-id="$BUILD_RUN"
INSTALLER_RUN=""
for _ in $(seq 1 30); do
  INSTALLER_RUN="$(gh run list --repo "$REPO" --workflow windows-installer.yml --event workflow_dispatch --limit 5 \
    --json databaseId,createdAt -q "[.[] | select(.createdAt >= \"$STARTED\")] | last | .databaseId // empty")"
  [[ -n "$INSTALLER_RUN" ]] && break
  sleep 5
done
[[ -z "$INSTALLER_RUN" ]] && echo "The windows-installer.yml run didn't start" >&2 && exit 1
gh run watch "$INSTALLER_RUN" --repo "$REPO" --exit-status --interval 15 >/dev/null

gh run download "$INSTALLER_RUN" --repo "$REPO" --name windows-installer-unsigned --dir "$WORK/installer"
INSTALLER="$(ls "$WORK"/installer/tom-ui-*-windows-*.exe)"
sign "$INSTALLER"

echo "--> Uploading the signed installer to the draft release"
gh release upload "$TAG" "$INSTALLER" --repo "$REPO" --clobber

echo
echo "Done. Review the draft release, then publish it:"
gh release view "$TAG" --repo "$REPO" --json url,assets -q '.url, (.assets[] | "  \(.name)")'
echo "  gh release edit $TAG --repo $REPO --draft=false"
