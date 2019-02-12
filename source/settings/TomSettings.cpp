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
