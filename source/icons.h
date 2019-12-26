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
    inline const QIcon themeIcon(const QString& /*path*/) {
        return QIcon();
    }
#else
    #ifdef Q_OS_WIN
        inline const QIcon themeIcon(const QString& path) {
            return icon(path);
        }
    #else
        inline const QIcon themeIcon(const QString &path) {
            return QIcon::fromTheme(path);
        }
    #endif
#endif

    inline const QIcon exit() {
        return themeIcon("application-exit");
    }

    inline const QIcon refreshData() {
        return themeIcon("view-refresh");
    }

    inline const QIcon resetData() {
        return themeIcon("edit-clear");
    }

    inline const QIcon windowHide() {
        return themeIcon("window-close");
    }

    inline const QIcon about() {
        return themeIcon("help-about");
    }

    inline const QIcon startTimer() {
        return icon("timer-start");
    }

    inline const QIcon stopTimer() {
#ifdef Q_OS_MAC
        return icon("osx/trayicon-started.svg");
#else
        return icon("timer-stop");
#endif
    }

    inline const QIcon timeEntryArchive() {
#ifdef Q_OS_MAC
        return icon("document-save");
#else
        return themeIcon("document-save");
#endif
    }

    inline const QIcon report() {
        return icon("document-send");
    }

    inline const QIcon importData() {
        return themeIcon("document-import");
    }

    inline const QIcon importTableData() {
        return themeIcon("x-office-spreadsheet");
    }

    inline const QIcon projectLookup() {
        return icon("system-search");
    }

    inline const QIcon projectNew() {
        return icon("document-new");
    }

    inline QIcon projectEdit() {
        return icon("document-properties");
    }

    inline QIcon frameEdit() {
        return icon("document-properties");
    }

    inline QIcon projectRemove() {
        return icon("edit-delete");
    }

    inline QIcon projectSelectActive() {
        return icon("go-jump");
    }

    inline const QIcon projectStart() {
        return icon("timer-start");
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

    inline const QIcon windowProjectsFocus() {
        return icon("go-first");
    }

    inline const QIcon windowTimeEntriesFocus() {
        return icon("go-last");
    }

    inline const QIcon keySequenceReset() {
        return icon("edit-clear");
    }

    inline const QIcon LogoSmall() {
        return icon("logo32.png");
    }

    inline const QIcon LogoLarge() {
        return icon("logo64.png");
    }
}

#endif //TOM_UI_ICONS_H
