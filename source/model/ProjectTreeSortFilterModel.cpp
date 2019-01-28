#include <QtCore>

#include "ProjectTreeSortFilterModel.h"
#include "UserRoles.h"

ProjectTreeSortFilterModel::ProjectTreeSortFilterModel(QObject *parent) : QSortFilterProxyModel(parent) {
    setSortRole(UserRoles::SortValueRole);
}

bool ProjectTreeSortFilterModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const {
/*
    if (!source_left.isValid() || !source_right.isValid()) {
        return false;
    }

    QVariant sortLeft = source_left.data(SortValueRole);
    QVariant sortRight = source_right.data(SortValueRole);
    if (sortLeft.isValid() && sortRight.isValid()) {
        // duration column is using qint64
        if (sortLeft.type() == QVariant::LongLong && sortRight.type() == QVariant::LongLong) {
            return sortLeft.toLongLong(nullptr) < sortRight.toLongLong(nullptr);
        }
    }
*/

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}

void ProjectTreeSortFilterModel::sort(int column, Qt::SortOrder order) {
//    const QModelIndexList &list = persistentIndexList();
//    qDebug() << "sort" << column << order << "indexes" << list;
//
//    for (auto i : list) {
//        qDebug() << i.data(Qt::DisplayRole) << i.data(UserRoles::SortValueRole);
//    }

    QSortFilterProxyModel::sort(column, order);
}
