#include "AbstractShortcutTableModel.h"

AbstractShortcutTableModel::AbstractShortcutTableModel(QObject *parent) : QAbstractTableModel(parent) {

}

QVariant AbstractShortcutTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) {
            return tr("Action");
        }
        if (section == 1) {
            return tr("Shortcut");
        }
    }
    return QVariant();
}

int AbstractShortcutTableModel::columnCount(const QModelIndex &) const {
    return 2;
}

QModelIndex AbstractShortcutTableModel::parent(const QModelIndex &) const {
    return {};
}
