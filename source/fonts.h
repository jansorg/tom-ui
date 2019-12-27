#ifndef TOM_UI_FONTS_H
#define TOM_UI_FONTS_H

#include <QtGui/QFont>

namespace Fonts {
    inline bool useMonospaceFont() {
#ifdef Q_OS_MAC
        return true;
#else
        return false;
#endif
    }

    inline QFont monospaceFont() {
        QFont font("Monospace");
        // it would be better to use tabular figures, when available
        // but Qt isn't supporting to configure opentype features, afaik
        font.setStyleHint(QFont::Monospace);
        return font;
    }
}

#endif //TOM_UI_FONTS_H
