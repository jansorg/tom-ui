//
// Created by jansorg on 28.01.19.
//

#include "reportsplitmodel.h"

ReportSplitModel::ReportSplitModel(QObject* parent) : QStringListModel(QStringList() << "year" << "month" << "day" << "project", parent){
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
        } else if (item == "day") {
            return tr("Day");
        } else if (item == "project") {
            return tr("Project");
        }
    }

    if (role == Qt::CheckStateRole) {
        return _checked.contains(index) ? Qt::Checked : Qt::Unchecked;
    }

    return QStringListModel::data(index, role);
}

bool ReportSplitModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid()) {
        return false;
    }

    if (role == Qt::CheckStateRole) {
        if (value == Qt::Checked) {
            _checked.insert(index);
        } else {
            _checked.remove(index);
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
        if (_checked.contains(rowIndex)){
            enabled << rowIndex.data(Qt::EditRole).toString();
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