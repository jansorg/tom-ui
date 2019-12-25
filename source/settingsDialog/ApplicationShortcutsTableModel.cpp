#include <utility>
#include <QtCore/QSettings>
#include <source/settings/TomSettings.h>

#include "ApplicationShortcutsTableModel.h"

ApplicationShortcutsTableModel::ApplicationShortcutsTableModel(QList<QAction *> actions, TomSettings *settings, QObject *parent) : AbstractShortcutTableModel(parent),
                                                                                                                                   _actions(std::move(actions)),
                                                                                                                                   _settings(settings) {
}

QKeySequence ApplicationShortcutsTableModel::getActionKeySequence(const QModelIndex &index) {
    if (!index.isValid()) {
        return QKeySequence();
    }

    QAction *action = _actions[index.row()];
    return action->shortcut();
}

void ApplicationShortcutsTableModel::updateKeySequence(const QModelIndex &index, const QKeySequence &keySequence) {
    if (!index.isValid()) {
        return;
    }

    auto *action = _actions[index.row()];
    action->setShortcut(keySequence);

    _settings->saveAction(action);

    emit dataChanged(index.siblingAtColumn(0), index.siblingAtColumn(1));
}

int ApplicationShortcutsTableModel::rowCount(const QModelIndex &) const {
    return _actions.size();
}

QVariant ApplicationShortcutsTableModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        QAction *action = _actions[index.row()];

        if (index.column() == 0) {
            return action->text().remove("&");
        }
        if (index.column() == 1) {
            return action->shortcut().toString();
        }
    }
    return QVariant();
}
