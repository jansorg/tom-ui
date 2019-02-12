#include "TomSettings.h"

void TomSettings::triggerUpdate() {
    emit onShowArchivedEntriesChanged(showArchivedEntries());
}

void TomSettings::setShowArchivedEntries(bool show) {
    bool changed = showArchivedEntries() != show;
    
    QSettings settings;
    settings.setValue("entries/showArchived", show);

    if (changed) {
        emit onShowArchivedEntriesChanged(show);
    }
}

bool TomSettings::showArchivedEntries() {
    QSettings settings;
    return settings.value("entries/showArchived", true).toBool();
}

QPoint TomSettings::mainWindowPos() {
    QSettings settings;
    return settings.value("mainwindow/pos", QPoint(200, 200)).toPoint();
}

QSize TomSettings::mainWindowSize() {
    QSettings settings;
    return settings.value("mainwindow/size", QSize(640, 480)).toSize();
}

void TomSettings::setMainWindowSize(const QSize &size) {
    QSettings settings;
    settings.setValue("mainwindow/size", size);
}

void TomSettings::setMainWindowPos(const QPoint &pos) {
    QSettings settings;
    settings.setValue("mainwindow/pos", pos);
}
