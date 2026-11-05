# Development

`tom-ui` is a Qt 6 application built with CMake. At runtime it needs the [`tom`](https://github.com/jansorg/tom) command line application.

## Requirements

- CMake 3.16 or later
- A C++20 compiler: GCC or Clang on Linux, Xcode on macOS, MSVC (Visual Studio) on Windows
- Qt 6.4 or later with the modules Widgets, Svg, LinguistTools and WebEngineWidgets (WebEngine is only needed for reports, see `ENABLE_REPORTS` below)
- Linux only: Qt DBus (part of qt6-base) and the X11 development headers

### Ubuntu 24.04

```bash
sudo apt install cmake g++ make libgl1-mesa-dev libx11-dev \
  qt6-base-dev qt6-base-dev-tools qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools \
  qt6-webengine-dev qt6-webengine-dev-tools libqt6svg6-dev
```

### Arch Linux

```bash
sudo pacman -S cmake gcc qt6-base qt6-svg qt6-tools qt6-webengine libx11
```

### macOS and Windows

Install Qt with the [Qt online installer](https://www.qt.io/download-qt-installer) or [aqtinstall](https://github.com/miurahr/aqtinstall), including the Qt WebEngine, Qt WebChannel and Qt Positioning modules. Pass the Qt directory to CMake with `-DCMAKE_PREFIX_PATH=<Qt dir>`, e.g. `~/Qt/6.11.3/macos`.

## Building

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

CMake options:

- `-DENABLE_REPORTS=OFF` builds without Qt WebEngine and without the report preview.

## Running

```bash
./build/tom-ui --tom /path/to/tom
```

- `--tom <path>`: path to the `tom` executable. On Linux the default is `tom` from `PATH`. On macOS and Windows, the default is the `tom` binary next to the `tom-ui` executable.
- `--configName <name>`: uses separate settings, useful to test without touching your own configuration.
- `--bash`: treat the `tom` executable as a Bash script.

## Global shortcuts

- X11, macOS and Windows: the native APIs of the platform are used (`qxt/`).
- Wayland: shortcuts are registered with the [XDG desktop portal](https://flatpak.github.io/xdg-desktop-portal/docs/doc-org.freedesktop.portal.GlobalShortcuts.html) (`portal/`). The desktop environment manages the actual key bindings; the shortcuts defined in tom-ui are only suggestions. This needs a portal with global shortcut support, e.g. KDE Plasma, GNOME 48+ or Hyprland. The app identifies itself as `tom-ui`, matching `resources/tom-ui.desktop`.

## Translations

Translations are stored in `translations/*.ts` and compiled into the application with every build. To update the `.ts` files after changing user-visible strings, run:

```bash
cmake --build build --target translations
```

Then translate the new strings with Qt Linguist (`linguist6 translations/tom_de.ts`) and commit the `.ts` file.

## Icons

`update-assets.sh` exports the SVG icons in `images/` to `resources/` and regenerates the application icons and `resources/resources.qrc`. It needs Inkscape and `png2icns`.

## Continuous integration

`.github/workflows/build.yml` builds packages for every push to `master` and every pull request:

| Platform | Runner | Qt | Artifact |
|---|---|---|---|
| Linux | Ubuntu 24.04 | 6.4.2 (Ubuntu packages) | `.deb`, depends on the `tom` package |
| macOS | macOS 26, Apple Silicon only | 6.11.3 | signed and notarized `.dmg`, bundles `tom` and `tom-pdf` |
| Windows | Windows Server 2025, MSVC | 6.10.3 | unsigned installer, bundles `tom.exe` and `tom-pdf.exe` |

- The bundled `tom` binaries are taken from the latest [tom release](https://github.com/jansorg/tom/releases). A manual run of the workflow can choose another release with the `tom-release` input.
- The build fails unless the bundled `tom` binaries are signed: a Developer ID signature on macOS (and arm64 binaries), an Authenticode signature on Windows.
- The macOS and Windows Qt versions are the latest that [install-qt-action](https://github.com/jurplel/install-qt-action) (aqtinstall 3.3) can install. Qt 6.12 changed the layout of its online repository, see [aqtinstall#1047](https://github.com/miurahr/aqtinstall/issues/1047). Update the versions in `build.yml` once that's fixed.

## Making a release

1. Set the new version in `version.txt`, without a trailing newline, then commit and push:

   ```bash
   printf '%s' 0.14.0 > version.txt
   git commit -m "Version 0.14.0" version.txt && git push
   ```

2. Tag and push the tag:

   ```bash
   git tag v0.14.0 && git push origin v0.14.0
   ```

   CI builds all packages and creates a **draft** release with the Linux and macOS packages.

3. Sign the Windows release with the code signing certificate on the YubiKey:

   ```bash
   deployment/windows/sign-windows-release.sh 0.14.0
   ```

   The script waits for the tag's build and signs `tom-ui.exe` locally. It then has the `windows-installer.yml` workflow build the installer with the signed executable, signs the installer and adds it to the draft release. It asks for the YubiKey PIV PIN; touch the key if it blinks.

4. Review the draft release on GitHub and publish it:

   ```bash
   gh release edit v0.14.0 --draft=false
   ```

### Windows signing setup

The private key never leaves the YubiKey, which is why the Windows files are signed locally and not in CI. The signing script needs:

- [`gh`](https://cli.github.com/), logged in with access to the repository
- [jsign](https://ebourg.github.io/jsign/) 5.0 or later
- `osslsigncode`, to verify the signatures
- the YubiKey PKCS#11 module `libykcs11` (package `yubico-piv-tool`)

Optional environment variables:

| Variable | Default | |
|---|---|---|
| `TOM_SIGN_ALIAS` | `X.509 Certificate for Digital Signature` (PIV slot 9c) | certificate on the YubiKey, see `ykman piv info` |
| `TOM_SIGN_TSA` | `http://timestamp.digicert.com` | RFC 3161 timestamp server |
| `TOM_SIGN_CA` | `/etc/ssl/certs/ca-certificates.crt` | CA bundle to verify the signatures |
| `TOM_SIGN_PIN` | prompted | YubiKey PIV PIN |

The uninstaller written by Inno Setup at install time is not signed.

### macOS signing

CI signs the app with the Developer ID certificate and the hardened runtime. Builds of pushes to `master`, of tags and of manual runs are also notarized and stapled. Pull requests are signed but not notarized. Builds without access to the secrets, such as pull requests from forks, are only signed ad-hoc.

Repository secrets:

| Secret | |
|---|---|
| `MACOS_SIGN_P12` | Developer ID Application certificate with its private key, as base64 encoded `.p12` (`base64 -i cert.p12`) |
| `MACOS_SIGN_PASSWORD` | password of the `.p12` file |
| `MACOS_NOTARY_KEY` | App Store Connect API key (`AuthKey_<id>.p8`), its content or base64 encoded |
| `MACOS_NOTARY_KEY_ID` | ID of the API key |
| `MACOS_NOTARY_ISSUER_ID` | issuer ID of the API key |

The Qt WebEngine helper process is signed with the entitlements shipped with Qt (`QtWebEngineProcess.entitlements`). The bundled `tom` binaries keep their own signatures.
