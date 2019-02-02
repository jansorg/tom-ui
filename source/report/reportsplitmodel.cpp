#include <QtCore>

#include "reportsplitmodel.h"

ReportSplitModel::ReportSplitModel(QObject *parent) : QStringListModel(QStringList() << "project" << "year" << "month" << "week" << "day", parent) {
}

Qt::ItemFlags ReportSplitModel::flags(const QModelIndex &index) const {
    const Qt::ItemFlags &flags = QStringListModel::flags(index);
    if (index.isValid()) {
        return flags | Qt::ItemIsUserCheckable;
    }
    return flags;
}

QVariant ReportSplitModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        QString item = stringList().at(index.row());
        if (item == "year") {
            return tr("Year");
        } else if (item == "month") {
            return tr("Month");
        } else if (item == "week") {
            return tr("Week");
        } else if (item == "day") {
            return tr("Day");
        } else if (item == "project") {
            return tr("Project");
        }
    }

    if (role == Qt::CheckStateRole) {
        return _checked.contains(index.data(Qt::EditRole).toString()) ? Qt::Checked : Qt::Unchecked;
    }

    return QStringListModel::data(index, role);
}

bool ReportSplitModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationRow) {
    bool ok = hasIndex(sourceRow, 0, sourceParent);
    if (!ok || count != 1 || destinationRow < 0) {
        return false;
    }

    auto list = stringList();
    const auto& rowData = list.at(sourceRow);
    list.removeAt(sourceRow);
    list.insert(destinationRow, rowData);
    setStringList(list);
    return true;
}

bool ReportSplitModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid()) {
        return false;
    }

    if (role == Qt::CheckStateRole) {
        auto rowValue = index.data(Qt::EditRole).toString();
        if (value == Qt::Checked) {
            _checked.insert(rowValue);
        } else {
            _checked.remove(rowValue);
        }
        emit itemStateChanged();
        emit dataChanged(index, index);
        return true;
    }

    return QStringListModel::setData(index, value, role);
}

QStringList ReportSplitModel::checkedItems() {
    QStringList enabled;
    for (int i = 0; i < rowCount(); i++) {
        const QModelIndex &rowIndex = createIndex(i, 0);
        const QVariant &rowValue = rowIndex.data(Qt::EditRole);
        if (_checked.contains(rowValue.toString())) {
            enabled << rowValue.toString();
        }
    }
    return enabled;
}

void ReportSplitModel::setCheckedItems(const QStringList &items) {
    const QStringList &source = stringList();
    for (const auto &item :items) {
        int row = source.indexOf(item);
        if (row >= 0) {
            setData(createIndex(row, 0), Qt::Checked, Qt::CheckStateRole);
        }
    }
}