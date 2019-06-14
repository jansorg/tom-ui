#include "ProjectTreeRootItem.h"

#include <QApplication>

ProjectTreeRootItem::ProjectTreeRootItem(const ProjectStatusManager *statusManager, ProjectTreeItem *parent) : ProjectTreeItem(Project(), statusManager, parent) {}

ProjectTreeRootItem::~ProjectTreeRootItem() = default;

QVariant ProjectTreeRootItem::data(int column) const {
    switch (column) {
        case COL_NAME:
            return QApplication::tr("All projects");
        case COL_YESTERDAY:
            return _statusManager->getOverallStatus().yesterdayTotal.formatShort();
        case COL_TODAY:
            return _statusManager->getOverallStatus().dayTotal.formatShort();
        case COL_WEEK:
            return _statusManager->getOverallStatus().weekTotal.formatShort();
        case COL_MONTH:
            return _statusManager->getOverallStatus().monthTotal.formatShort();
        case COL_YEAR:
            return _statusManager->getOverallStatus().yearTotal.formatShort();
        case COL_TOTAL:
            return _statusManager->getOverallStatus().allTotal.formatShort();
        default:
            return QVariant();
    }
}

bool ProjectTreeRootItem::setData(int, const QVariant &) {
    return false;
}
