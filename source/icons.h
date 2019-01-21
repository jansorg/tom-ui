#ifndef TOM_UI_ICONS_H
#define TOM_UI_ICONS_H

#include <QIcon>

namespace Icons {
#ifdef Q_OS_MAC
    inline const QIcon themeIcon(const QString& path) {
        return QIcon();
    }
    inline const QIcon icon(const QString& path) {
        return QIcon(QString(":/icons/") + path);
    }
#else

    inline const QIcon icon(const QString &path) {
        return QIcon(QString(":/icons/") + path);
    }

    inline const QIcon themeIcon(const QString &path) {
        return QIcon::fromTheme(path/*, icon(path + ".svg")*/);
    }

#endif

    inline QIcon exit() {
        return themeIcon("application-exit");
    }

    inline QIcon about() {
        return themeIcon("help-about");
    }

    inline QIcon startTimer() {
        return icon("start-timer.svg");
    }

    inline QIcon stopTimer() {
        return icon("stop-timer.svg");
    }

    inline QIcon newProject() {
        return themeIcon("list-add");
    }

    inline const QIcon removeFrame() {
        return themeIcon("list-remove");
    }

    inline const QIcon Logo() {
        return icon("logo64.png");
    }
}

#endif //TOM_UI_ICONS_H
