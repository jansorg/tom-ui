#include "PaletteIconEngine.h"

#include <QFile>
#include <QGuiApplication>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QSvgRenderer>

PaletteIconEngine::PaletteIconEngine(const QString &svgPath) {
    QFile file(svgPath);
    if (file.open(QIODevice::ReadOnly)) {
        _svg = file.readAll();
    }
}

QByteArray PaletteIconEngine::coloredSvg(QIcon::Mode mode) const {
    const QPalette palette = QGuiApplication::palette();
    QColor color;
    switch (mode) {
        case QIcon::Disabled:
            color = palette.color(QPalette::Disabled, QPalette::WindowText);
            break;
        case QIcon::Selected:
            color = palette.color(QPalette::Active, QPalette::HighlightedText);
            break;
        default:
            color = palette.color(QPalette::Active, QPalette::WindowText);
            break;
    }

    QByteArray svg = _svg;
    svg.replace("#000000", color.name(QColor::HexRgb).toLatin1());
    if (color.alpha() < 255) {
        svg.replace("<svg ", QStringLiteral("<svg opacity=\"%1\" ").arg(color.alphaF()).toLatin1());
    }
    return svg;
}

void PaletteIconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State /*state*/) {
    QSvgRenderer renderer(coloredSvg(mode));
    renderer.render(painter, rect);
}

QPixmap PaletteIconEngine::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) {
    return scaledPixmap(size, mode, state, 1.0);
}

QPixmap PaletteIconEngine::scaledPixmap(const QSize &size, QIcon::Mode mode, QIcon::State state, qreal scale) {
    QPixmap pixmap(size * scale);
    pixmap.fill(Qt::transparent);
    {
        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        paint(&painter, QRect(QPoint(0, 0), pixmap.size()), mode, state);
    }
    pixmap.setDevicePixelRatio(scale);
    return pixmap;
}

QIconEngine *PaletteIconEngine::clone() const {
    return new PaletteIconEngine(*this);
}

QString PaletteIconEngine::key() const {
    return QStringLiteral("PaletteIconEngine");
}
