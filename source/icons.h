#ifndef TOM_UI_ICONS_H
#define TOM_UI_ICONS_H

#include <QIcon>

namespace Icons {
    inline const QIcon icon(const QString &name) {
        if (name.endsWith(".svg") || name.endsWith(".png")) {
            return QIcon(":icons/" + name);
        }
        if (QIcon::hasThemeIcon(name)) {
            return QIcon::fromTheme(name);
        }
        return QIcon(QString(":/icons/") + name + ".svg");
    }

#ifdef Q_OS_MAC
    inline const QIcon themeIcon(const QString& path) {
        return QIcon();
    }
#else

    inline const QIcon themeIcon(const QString &path) {
        return QIcon::fromTheme(path);
    }

#endif

    inline QIcon exit() {
        return themeIcon("application-exit");
    }

    inline QIcon about() {
        return themeIcon("help-about");
    }

    inline QIcon startTimer() {
        return icon("timer-start");
    }

    inline QIcon stopTimer() {
        return icon("timer-stop");
    }

    inline const QIcon removeFrame() {
        return themeIcon("edit-delete");
    }

    inline const QIcon importData() {
        return themeIcon("document-import");
    }

    inline QIcon projectNew() {
        return icon("document-new");
    }

    inline QIcon projectRemove() {
        return icon("edit-delete");
    }

    inline const QIcon projectStart() {
        return icon("timer-start");
    }

    inline const QIcon projectStop() {
        return icon("timer-stop");
    }

    inline const QIcon timeEntryDelete() {
        return icon("edit-delete");
    }

    inline QIcon activeProject() {
        return icon("project-active");
    }

    inline QIcon withActiveSubproject() {
        return icon("project-active-subproject");
    }

    inline const QIcon LogoSmall() {
        return icon("logo32.png");
    }

    inline const QIcon LogoLarge() {
        return icon("logo64.png");
    }
}

#endif //TOM_UI_ICONS_H
