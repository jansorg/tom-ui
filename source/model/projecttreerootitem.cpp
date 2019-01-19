//
// Created by jansorg on 19.01.19.
//

#include "projecttreerootitem.h"

ProjectTreeRootItem::ProjectTreeRootItem(const ProjectStatusManager *statusManager, ProjectTreeItem* parent) : ProjectTreeItem(Project(), statusManager, parent) {}

ProjectTreeRootItem::~ProjectTreeRootItem() = default;

QVariant ProjectTreeRootItem::data(int column) const {
    if (column == COL_NAME) {
        return "Overall";
    }
    return QVariant();
}

bool ProjectTreeRootItem::setData(int column, const QVariant &) {
    return false;
}