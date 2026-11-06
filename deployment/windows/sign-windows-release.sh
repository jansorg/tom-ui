#!/usr/bin/env bash
#
# Signs the Windows files of a draft GitHub release of tom-ui with the code signing
# certificate on a YubiKey.
#
# Usage: deployment/windows/sign-windows-release.sh <tag>, e.g. v0.14.0
#
# Workflow:
#   1. Create the draft release: gh workflow run release.yml --ref master -f version=0.14.0
#   2. Run this script with the YubiKey plugged in
#   3. Review and publish the draft release on GitHub
#
# The script
#   1. downloads the unsigned application archive tom-ui-<version>-windows-app.zip of the draft release,
#      signs tom-ui.exe (Authenticode, timestamped) and replaces the archive in the draft release
#   2. starts the windows-installer.yml workflow, which builds the installer from the archive
#      and attaches it to the draft release
#   3. signs the installer, replaces it in the draft release and removes the application archive
#
# The private key never leaves the YubiKey. Running the script again for the same draft release is safe
# as long as the application archive is still attached.
#
# Requirements: gh (logged in), jsign (needs java), zip, unzip, osslsigncode, the YubiKey PKCS#11 module (libykcs11, yubico-piv-tool).
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

TAG="${1:-}"
[[ -z "$TAG" ]] && echo "Usage: $0 <tag>, e.g. v0.14.0" >&2 && exit 1
TAG="v${TAG#v}"

for cmd in gh jsign osslsigncode zip unzip; do
  command -v "$cmd" >/dev/null || { echo "Missing required command: $cmd" >&2; exit 1; }
done

# Never modify a published release, its files may already have been downloaded
if [[ "$(gh release view "$TAG" --repo "$REPO" --json isDraft -q .isDraft)" != "true" ]]; then
  echo "Release $TAG is not a draft, refusing to modify it" >&2
  exit 1
fi

APP_ARCHIVE="$(gh release view "$TAG" --repo "$REPO" --json assets -q '.assets[].name | select(endswith("-windows-app.zip"))')"
if [[ -z "$APP_ARCHIVE" ]]; then
  echo "The draft release $TAG has no application archive tom-ui-<version>-windows-app.zip." >&2
  exit 1
fi

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

if [[ -z "${TOM_SIGN_PIN:-}" ]]; then
  read -rsp "YubiKey PIV PIN: " TOM_SIGN_PIN
  echo
fi
export TOM_SIGN_PIN

sign() {
  local file="$1"
  echo "--> Signing $(basename "$file") (touch the YubiKey if it blinks)"
  # --replace: signing a file again, e.g. when the script is run again, replaces the signature
  jsign --storetype YUBIKEY --storepass env:TOM_SIGN_PIN --alias "$ALIAS" \
    --tsaurl "$TSA" --tsmode RFC3161 --replace \
    --name "Tom" --url "https://github.com/$REPO" \
    "$file"
  osslsigncode verify -in "$file" -CAfile "$CA_BUNDLE" >/dev/null
  echo "    signature verified"
}

echo "--> Downloading $APP_ARCHIVE of release $TAG"
gh release download "$TAG" --repo "$REPO" --pattern "$APP_ARCHIVE" --dir "$WORK"
mkdir "$WORK/app"
unzip -q "$WORK/$APP_ARCHIVE" -d "$WORK/app"

sign "$WORK/app/bin/tom-ui.exe"
for exe in tom.exe tom-pdf.exe; do
  osslsigncode verify -in "$WORK/app/bin/$exe" -CAfile "$CA_BUNDLE" >/dev/null \
    || { echo "The bundled $exe is not signed" >&2; exit 1; }
done

echo "--> Replacing $APP_ARCHIVE of the draft release with the signed application"
rm "$WORK/$APP_ARCHIVE"
(cd "$WORK/app" && zip -q -r -X "$WORK/$APP_ARCHIVE" .)
gh release upload "$TAG" "$WORK/$APP_ARCHIVE" --repo "$REPO" --clobber

echo "--> Building the installer"
# the run name contains the request ID, which identifies the started run
REQUEST_ID="$(date -u +%Y%m%d%H%M%S)-$$"
gh workflow run windows-installer.yml --repo "$REPO" --ref "$TAG" -f tag="$TAG" -f request-id="$REQUEST_ID"
INSTALLER_RUN=""
for _ in $(seq 1 30); do
  INSTALLER_RUN="$(gh run list --repo "$REPO" --workflow windows-installer.yml --event workflow_dispatch --limit 20 \
    --json databaseId,displayTitle -q "[.[] | select(.displayTitle | endswith(\"($REQUEST_ID)\"))][0].databaseId // empty")"
  [[ -n "$INSTALLER_RUN" ]] && break
  sleep 5
done
[[ -z "$INSTALLER_RUN" ]] && echo "The windows-installer.yml run didn't start" >&2 && exit 1
echo "    https://github.com/$REPO/actions/runs/$INSTALLER_RUN"
gh run watch "$INSTALLER_RUN" --repo "$REPO" --exit-status --interval 15 >/dev/null

INSTALLER_NAME="${APP_ARCHIVE%-app.zip}"
INSTALLER_NAME="$(gh release view "$TAG" --repo "$REPO" --json assets \
  -q ".assets[].name | select(startswith(\"$INSTALLER_NAME-\") and endswith(\".exe\"))")"
[[ -z "$INSTALLER_NAME" ]] && echo "The installer wasn't attached to the draft release" >&2 && exit 1

echo "--> Downloading $INSTALLER_NAME of release $TAG"
mkdir "$WORK/installer"
gh release download "$TAG" --repo "$REPO" --pattern "$INSTALLER_NAME" --dir "$WORK/installer"
sign "$WORK/installer/$INSTALLER_NAME"

echo "--> Replacing $INSTALLER_NAME of the draft release with the signed installer"
gh release upload "$TAG" "$WORK/installer/$INSTALLER_NAME" --repo "$REPO" --clobber

echo "--> Removing $APP_ARCHIVE from the draft release"
gh release delete-asset "$TAG" "$APP_ARCHIVE" --repo "$REPO" --yes

echo
echo "Done. Review the draft release, then publish it:"
gh release view "$TAG" --repo "$REPO" --json url,assets -q '.url, (.assets[] | "  \(.name)")'
echo "  gh release edit $TAG --repo $REPO --draft=false"
