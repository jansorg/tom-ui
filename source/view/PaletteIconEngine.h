#ifndef TOM_UI_PALETTEICONENGINE_H
#define TOM_UI_PALETTEICONENGINE_H

#include <QIconEngine>

/**
 * Renders a monochrome SVG icon in the text color of the application palette,
 * so that it follows light and dark mode like the native icons of macOS and Windows.
 * The SVG must be drawn in #000000, which is replaced at paint time.
 */
class PaletteIconEngine : public QIconEngine {
public:
    explicit PaletteIconEngine(const QString &svgPath);

    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) override;

    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) override;

    QPixmap scaledPixmap(const QSize &size, QIcon::Mode mode, QIcon::State state, qreal scale) override;

    QIconEngine *clone() const override;

    QString key() const override;

private:
    QByteArray coloredSvg(QIcon::Mode mode) const;

    QByteArray _svg;
};

#endif //TOM_UI_PALETTEICONENGINE_H
