#include "ProjectTreeSortFilterModel.h"
#include "UserRoles.h"

ProjectTreeSortFilterModel::ProjectTreeSortFilterModel(QObject *parent) : QSortFilterProxyModel(parent) {}

bool ProjectTreeSortFilterModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const {
    QVariant sortLeft = source_left.data(SortValueRole);
    QVariant sortRight = source_right.data(SortValueRole);
    if (sortLeft.isValid() && sortRight.isValid()) {
        // duration column is using qint64
        if (sortLeft.type() == QVariant::LongLong && sortRight.type() == QVariant::LongLong) {
            return sortLeft.toLongLong(nullptr) < sortRight.toLongLong(nullptr);
        }
    }

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}
