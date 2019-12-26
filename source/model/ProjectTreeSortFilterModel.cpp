#include <QtCore>

#include "ProjectTreeSortFilterModel.h"
#include "UserRoles.h"

ProjectTreeSortFilterModel::ProjectTreeSortFilterModel(QObject *parent) : QSortFilterProxyModel(parent) {
    setSortRole(UserRoles::SortValueRole);
}

bool ProjectTreeSortFilterModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const {
    return QSortFilterProxyModel::lessThan(source_left, source_right);
}

void ProjectTreeSortFilterModel::sort(int column, Qt::SortOrder order) {
    QSortFilterProxyModel::sort(column, order);
}
