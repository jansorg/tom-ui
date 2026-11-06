#include "FrameTableSortFilterModel.h"
#include "FrameTableViewModel.h"
#include "UserRoles.h"

FrameTableSortFilterModel::FrameTableSortFilterModel(QObject *parent) : QSortFilterProxyModel(parent) {}

bool FrameTableSortFilterModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const {
    QVariant sortLeft = source_left.data(SortValueRole);
    QVariant sortRight = source_right.data(SortValueRole);
    if (sortLeft.isValid() && sortRight.isValid()) {
        // duration column is using qint64
        if (sortLeft.typeId() == QMetaType::LongLong && sortRight.typeId() == QMetaType::LongLong) {
            return sortLeft.toLongLong(nullptr) < sortRight.toLongLong(nullptr);
        }

        //start and end are using QDateTime, but show QTime
        if (sortLeft.typeId() == QMetaType::QDateTime && sortRight.typeId() == QMetaType::QDateTime) {
            return sortLeft.toDateTime() < sortRight.toDateTime();
        }
    }

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}
