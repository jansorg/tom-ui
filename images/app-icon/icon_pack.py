#!/usr/bin/env python3
"""Packs rendered PNG files into the icon containers of macOS and Windows,
without needing iconutil, libicns or icoutils.

  icon_pack.py icns output.icns icon_16.png ... icon_1024.png
      Every size used by macOS gets its own entry, including the Retina (@2x) entries.

  icon_pack.py ico output.ico bitmaps.ico icon_256.png
      Takes the entries of bitmaps.ico (e.g. written by ImageMagick, sizes below 256 px as bitmaps)
      and appends the PNG as the PNG-compressed 256 px entry, as recommended for Windows.

The pixel size of each PNG is read from its header.
"""
import struct
import sys

# pixel size -> ICNS types which store a PNG of that size
ICNS_TYPES = {
    16: [b"icp4"],
    32: [b"icp5", b"ic11"],  # 32x32, 16x16@2x
    64: [b"icp6", b"ic12"],  # 64x64, 32x32@2x
    128: [b"ic07"],
    256: [b"ic08", b"ic13"],  # 256x256, 128x128@2x
    512: [b"ic09", b"ic14"],  # 512x512, 256x256@2x
    1024: [b"ic10"],  # 512x512@2x
}


def read_png(path):
    with open(path, "rb") as f:
        data = f.read()
    if data[:8] != b"\x89PNG\r\n\x1a\n":
        raise SystemExit(f"{path}: not a PNG file")
    width, height = struct.unpack(">II", data[16:24])
    if width != height:
        raise SystemExit(f"{path}: icon is not square: {width}x{height}")
    return width, data


def icns(output, pngs):
    entries = b""
    for path in pngs:
        size, data = read_png(path)
        if size not in ICNS_TYPES:
            raise SystemExit(f"{path}: unsupported icon size {size}")
        for icns_type in ICNS_TYPES[size]:
            entries += icns_type + struct.pack(">I", len(data) + 8) + data
    with open(output, "wb") as f:
        f.write(b"icns" + struct.pack(">I", len(entries) + 8) + entries)


def ico(output, bitmaps, png):
    with open(bitmaps, "rb") as f:
        source = f.read()
    reserved, kind, count = struct.unpack("<HHH", source[:6])
    if reserved != 0 or kind != 1:
        raise SystemExit(f"{bitmaps}: not an ICO file")

    images = []  # (width, height, colors, planes, bpp, data)
    for i in range(count):
        w, h, colors, _, planes, bpp, size, offset = struct.unpack("<BBBBHHII", source[6 + 16 * i:22 + 16 * i])
        if w == 0:  # 256 px, replaced by the PNG
            continue
        images.append((w, h, colors, planes, bpp, source[offset:offset + size]))

    size, data = read_png(png)
    if size != 256:
        raise SystemExit(f"{png}: expected 256 px, got {size} px")
    images.append((0, 0, 0, 1, 32, data))

    header = struct.pack("<HHH", 0, 1, len(images))
    offset = 6 + 16 * len(images)
    directory, payload = b"", b""
    for w, h, colors, planes, bpp, data in images:
        directory += struct.pack("<BBBBHHII", w, h, colors, 0, planes, bpp, len(data), offset + len(payload))
        payload += data
    with open(output, "wb") as f:
        f.write(header + directory + payload)


def main():
    if len(sys.argv) >= 4 and sys.argv[1] == "icns":
        icns(sys.argv[2], sys.argv[3:])
    elif len(sys.argv) == 5 and sys.argv[1] == "ico":
        ico(sys.argv[2], sys.argv[3], sys.argv[4])
    else:
        raise SystemExit(__doc__)


if __name__ == "__main__":
    main()
