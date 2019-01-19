#include <QStringList>
#include <QtCore>

#include "project_tree_item.h"

ProjectTreeItem::ProjectTreeItem(const Project &project, const ProjectStatusManager *statusManager, ProjectTreeItem *parent) : _project(project),
                                                                                                                               _statusManager(statusManager),
                                                                                                                               _parentItem(parent) {
    refreshWith(project);
}

ProjectTreeItem::~ProjectTreeItem() {
    qDeleteAll(_childItems);
}

void ProjectTreeItem::appendChild(ProjectTreeItem *item) {
    _childItems.append(item);
}

ProjectTreeItem *ProjectTreeItem::child(int row) {
    return _childItems.value(row);
}

int ProjectTreeItem::childCount() const {
    return _childItems.count();
}

QVariant ProjectTreeItem::data(int column) const {
    switch (column) {
        case COL_NAME:
            return _projectName;
        case COL_DAY:
            return _statusManager->getStatus(_project.getID()).dayTotal.formatShort();
        case COL_WEEK:
            return _statusManager->getStatus(_project.getID()).weekTotal.formatShort();
        case COL_MONTH:
            return _statusManager->getStatus(_project.getID()).monthTotal.formatShort();
        default:
            return QVariant();
    }
}

bool ProjectTreeItem::setData(int column, const QVariant &value) {
    if (column == COL_NAME) {
        _projectName = value.toString();
        return true;
    }
    return false;
}

ProjectTreeItem *ProjectTreeItem::parentItem() {
    return _parentItem;
}

int ProjectTreeItem::row() const {
    if (_parentItem) {
        return _parentItem->_childItems.indexOf(const_cast<ProjectTreeItem *>(this));
    }

    return 0;
}

const Project &ProjectTreeItem::getProject() const {
    return _project;
}

void ProjectTreeItem::refreshWith(const Project &project) {
    _project = project;
    _projectName = project.getShortName();
}
