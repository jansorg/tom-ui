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

bool TomSettings::openMainWindowAtStartup() {
    QSettings settings;
    return settings.value("mainWindow/showAtStartup", true).toBool();
}

void TomSettings::setOpenMainWindowAtStartup(bool show) {
    QSettings settings;
    settings.setValue("mainWindow/showAtStartup", show);
}

void TomSettings::loadAction(QAction *action) {
    if (!action->objectName().isEmpty()) {
        QSettings settings;
        const QVariant &value = settings.value(QString("shortcut/%1").arg(action->objectName()));
        if (value.isValid() && value.canConvert<QKeySequence>()) {
            QKeySequence keySequence = value.value<QKeySequence>();
            action->setShortcut(keySequence);
        }

        if (action->isCheckable()) {
            const QVariant &checkedValue = settings.value(QString("mainwindow/%1").arg(action->objectName()));
            if (checkedValue.isValid()) {
                action->setChecked(checkedValue.toBool());
            }
        }
    }
}

void TomSettings::saveAction(QAction *action) {
    if (!action->objectName().isEmpty()) {
        QSettings settings;
        settings.setValue(QString("shortcut/%1").arg(action->objectName()), action->shortcut());

        if (action->isCheckable()) {
            settings.setValue(QString("mainwindow/%1").arg(action->objectName()), action->isChecked());
        }
    }
}
