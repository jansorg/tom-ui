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

    inline const QIcon exit() {
        return themeIcon("application-exit");
    }

    inline const QIcon about() {
        return themeIcon("help-about");
    }

    inline const QIcon startTimer() {
        return icon("timer-start");
    }

    inline const QIcon stopTimer() {
        return icon("timer-stop");
    }

    inline const QIcon removeFrame() {
        return icon("edit-delete");
    }

    inline const QIcon archiveFrame() {
        return icon("archive");
    }

    inline const QIcon importData() {
        return themeIcon("document-import");
    }

    inline const QIcon projectNew() {
        return icon("document-new");
    }

    inline QIcon projectEdit() {
        return icon("document-properties");
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

    inline const QIcon activeProject() {
        return icon("project-active");
    }

    inline const QIcon withActiveSubproject() {
        return icon("project-active-subproject");
    }

    inline const QIcon showMainWindow() {
        return icon("window");
    }

    inline const QIcon LogoSmall() {
        return icon("logo32.png");
    }

    inline const QIcon LogoLarge() {
        return icon("logo64.png");
    }
}

#endif //TOM_UI_ICONS_H
