#include "GlobalShortcutsTableModel.h"

GlobalShortcutsTableModel::GlobalShortcutsTableModel(GlobalShortcuts *shortcuts, QObject *parent) : AbstractShortcutTableModel(parent),
                                                                                                    _shortcuts(shortcuts) {
    connect(shortcuts, &GlobalShortcuts::assignedTriggersChanged, this, [this] {
        if (rowCount(QModelIndex()) > 0) {
            emit dataChanged(index(0, 1), index(rowCount(QModelIndex()) - 1, 1));
        }
    });

}

QKeySequence GlobalShortcutsTableModel::getActionKeySequence(const QModelIndex &index) {
    return _shortcuts->globalShortcuts()[index.row()]->shortcut();
}

void GlobalShortcutsTableModel::updateKeySequence(const QModelIndex &index, const QKeySequence &keySequence) {
    GlobalShortcut *shortcut = _shortcuts->globalShortcuts()[index.row()];
    shortcut->setShortcut(keySequence);

    QSettings settings;
    shortcut->save(settings);
}

int GlobalShortcutsTableModel::rowCount(const QModelIndex &) const {
    return _shortcuts->globalShortcuts().size();
}

QVariant GlobalShortcutsTableModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        GlobalShortcut *shortcut = _shortcuts->globalShortcuts()[index.row()];
        if (index.column() == 0) {
            return shortcut->label();
        }
        if (index.column() == 1) {
            if (_shortcuts->isManagedByDesktop()) {
                // the desktop environment decides about the actual key binding
                const QString &assigned = _shortcuts->assignedTrigger(shortcut);
                return assigned.isEmpty() ? tr("Not assigned") : assigned;
            }
            return shortcut->shortcut();
        }
    }
    return QVariant();
}
