#!/usr/bin/env python3
"""Generates the SVG sources of the application icon, one variant per platform.

The glyph is a "T" which is also a stopwatch: the crossbar is the running segment of a timer ring,
the stem is the hand and the crown sits on top of the ring.

Variants:
  tom-macos.svg     macOS: 1024 px canvas, 824 px continuous-corner plate with a drop shadow in the margin
                    (Apple's app icon grid since macOS 11)
  tom-windows.svg   Windows: plate filling the canvas almost completely, used for 48 px and above
  tom-linux.svg     Linux: plate with the proportions of the GNOME icon grid and a thin base below it,
                    installed as hicolor/scalable/apps/tom-ui.svg
  tom-small.svg     16-32 px for Windows and Linux: no timer track, no crown, thicker strokes
  tom-symbolic.svg  monochrome 16 px icon for GNOME and KDE, recolored by the desktop

Run update-assets.sh after editing this file.
"""
import math
import os
import sys

NAVY_TOP = "#24427f"
NAVY_BOTTOM = "#0e1d44"
CORAL_TOP = "#ff7a5c"
CORAL_BOTTOM = "#f2434f"


def squircle(cx, cy, half, n=4.0, steps=256):
    """Superellipse |x|^n + |y|^n = 1, which approximates Apple's continuous-corner shape."""
    points = []
    for i in range(steps):
        t = 2 * math.pi * i / steps
        c, s = math.cos(t), math.sin(t)
        x = math.copysign(abs(c) ** (2 / n), c) * half
        y = math.copysign(abs(s) ** (2 / n), s) * half
        points.append(f"{cx + x:.2f},{cy + y:.2f}")
    return "M" + " L".join(points) + " Z"


def arc(cx, cy, r, a0, a1):
    """Arc path, angles in degrees with 0 at 12 o'clock, clockwise."""

    def point(a):
        rad = math.radians(a - 90)
        return cx + r * math.cos(rad), cy + r * math.sin(rad)

    x0, y0 = point(a0)
    x1, y1 = point(a1)
    large = 1 if (a1 - a0) % 360 > 180 else 0
    return f"M{x0:.2f},{y0:.2f} A{r:.2f},{r:.2f} 0 {large} 1 {x1:.2f},{y1:.2f}"


class Glyph:
    """Geometry of the glyph. s is the scale relative to an 824 px plate."""

    def __init__(self, cx, cy, s, simple=False):
        self.cx, self.cy, self.s, self.simple = cx, cy, s, simple
        self.r = 262 * s
        self.w = (92 if simple else 76) * s
        self.top = cy - self.r
        self.bottom = cy + (150 if simple else 120) * s

    def gradient(self):
        y0 = self.top - self.w / 2
        y1 = self.bottom + self.w / 2
        return (f'<linearGradient id="accent" gradientUnits="userSpaceOnUse" x1="0" y1="{y0:.2f}" x2="0" y2="{y1:.2f}">'
                f'<stop offset="0" stop-color="{CORAL_TOP}"/><stop offset="1" stop-color="{CORAL_BOTTOM}"/>'
                f'</linearGradient>')

    def elements(self, paint="url(#accent)", track=True, crown=True, crown_paint="#ffffff"):
        cx, cy, s, r, w = self.cx, self.cy, self.s, self.r, self.w
        parts = []
        if track:
            parts.append(f'<circle cx="{cx:.2f}" cy="{cy:.2f}" r="{r:.2f}" fill="none" '
                         f'stroke="#ffffff" stroke-opacity="0.16" stroke-width="{w:.2f}"/>')
        parts.append(f'<path d="{arc(cx, cy, r, -64, 64)}" fill="none" stroke="{paint}" '
                     f'stroke-width="{w:.2f}" stroke-linecap="round"/>')
        parts.append(f'<path d="M{cx:.2f},{self.top + w / 2:.2f} L{cx:.2f},{self.bottom:.2f}" fill="none" '
                     f'stroke="{paint}" stroke-width="{w:.2f}" stroke-linecap="round"/>')
        if crown:
            cw, ch = 120 * s, 46 * s
            y = self.top - w / 2 - ch - 26 * s
            parts.append(f'<rect x="{cx - cw / 2:.2f}" y="{y:.2f}" width="{cw:.2f}" height="{ch:.2f}" '
                         f'rx="{ch / 2:.2f}" fill="{crown_paint}" fill-opacity="0.9"/>')
        return parts


def plate_defs(y0, y1):
    return (f'<linearGradient id="plate" gradientUnits="userSpaceOnUse" x1="0" y1="{y0:.2f}" x2="0" y2="{y1:.2f}">'
            f'<stop offset="0" stop-color="{NAVY_TOP}"/><stop offset="1" stop-color="{NAVY_BOTTOM}"/>'
            f'</linearGradient>'
            f'<linearGradient id="sheen" gradientUnits="userSpaceOnUse" x1="0" y1="{y0:.2f}" x2="0" y2="{y1:.2f}">'
            f'<stop offset="0" stop-color="#ffffff" stop-opacity="0.14"/>'
            f'<stop offset="0.5" stop-color="#ffffff" stop-opacity="0"/>'
            f'</linearGradient>')


def svg(size, defs, elements):
    body = "\n  ".join(elements)
    defs = "\n    ".join(defs)
    return (f'<svg xmlns="http://www.w3.org/2000/svg" width="{size}" height="{size}" viewBox="0 0 {size} {size}">\n'
            f'  <defs>\n    {defs}\n  </defs>\n  {body}\n</svg>\n')


def macos():
    size, half = 1024, 412
    shape = squircle(512, 512, half)
    glyph = Glyph(512, 540, 1.0)
    shadow = ('<filter id="shadow" x="-20%" y="-20%" width="140%" height="140%">'
              '<feGaussianBlur in="SourceAlpha" stdDeviation="14"/><feOffset dy="12"/>'
              '<feComponentTransfer><feFuncA type="linear" slope="0.35"/></feComponentTransfer>'
              '<feMerge><feMergeNode/><feMergeNode in="SourceGraphic"/></feMerge></filter>')
    return svg(size, [plate_defs(100, 924), glyph.gradient(), shadow],
               [f'<path d="{shape}" fill="url(#plate)" filter="url(#shadow)"/>',
                f'<path d="{shape}" fill="url(#sheen)"/>'] + glyph.elements())


def rounded(size, x, y, w, h, radius, simple=False, base=0.0):
    """Rounded plate with the glyph. base is the height of the darker base below the plate."""
    s = w / 824
    glyph = Glyph(x + w / 2, y + h / 2 + 28 * s, s, simple=simple)
    elements = []
    if base:
        elements.append(f'<rect x="{x}" y="{y + base:.2f}" width="{w}" height="{h}" rx="{radius}" '
                        f'fill="#08122b"/>')
    elements += [f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="{radius}" fill="url(#plate)"/>',
                 f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="{radius}" fill="url(#sheen)"/>']
    elements += glyph.elements(track=not simple, crown=not simple)
    return svg(size, [plate_defs(y, y + h), glyph.gradient()], elements)


def symbolic():
    # 16 px pixel-aligned, a single color which the desktop replaces with the foreground color of the theme
    cx, cy, r = 8, 9.5, 5.75
    color = "#2e3436"
    return svg(16, [], [
        f'<circle cx="{cx}" cy="{cy}" r="{r}" fill="none" stroke="{color}" stroke-opacity="0.35" stroke-width="1.5"/>',
        f'<path d="{arc(cx, cy, r, -64, 64)}" fill="none" stroke="{color}" stroke-width="2" stroke-linecap="round"/>',
        f'<path d="M{cx},{cy - r + 1} L{cx},12.5" fill="none" stroke="{color}" stroke-width="2" stroke-linecap="round"/>',
        f'<rect x="6.5" y="0.5" width="3" height="1.5" rx="0.75" fill="{color}"/>',
    ])


def main():
    out = sys.argv[1] if len(sys.argv) > 1 else os.path.dirname(os.path.abspath(__file__))
    files = {
        "tom-macos.svg": macos(),
        # Windows 11 app icons use nearly the whole canvas
        "tom-windows.svg": rounded(256, 8, 8, 240, 240, 52),
        # GNOME icon grid: 128 px canvas with a 104 px square plate, the base adds depth
        "tom-linux.svg": rounded(128, 12, 10, 104, 104, 22, base=4),
        "tom-small.svg": rounded(32, 1, 1, 30, 30, 6, simple=True),
        "tom-symbolic.svg": symbolic(),
    }
    for name, content in files.items():
        with open(os.path.join(out, name), "w") as f:
            f.write(content)


if __name__ == "__main__":
    main()
