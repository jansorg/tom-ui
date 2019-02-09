#include <QStringList>
#include <QtCore>

#include "ProjectTreeItem.h"

ProjectTreeItem::ProjectTreeItem(const Project &project, const ProjectStatusManager *statusManager, ProjectTreeItem *parent) : _project(project),
                                                                                                                               _statusManager(statusManager),
                                                                                                                               _parentItem(parent) {
    refreshWith(project);
}

ProjectTreeItem::~ProjectTreeItem() {
    reset();
}

void ProjectTreeItem::appendChild(ProjectTreeItem *item) {
    _childItems.append(item);
    item->_parentItem = this;
}

void ProjectTreeItem::removeChild(ProjectTreeItem *item) {
    _childItems.removeOne(item);
    item->_parentItem = nullptr;
}

ProjectTreeItem *ProjectTreeItem::childAt(int row) {
    return _childItems.value(row);
}

int ProjectTreeItem::childCount() const {
    return _childItems.count();
}

qint64 ProjectTreeItem::sortData(int column) const {
    switch (column) {
        case COL_DAY:
            return _statusManager->getStatus(_project.getID()).dayTotal.asMillis();
        case COL_WEEK:
            return _statusManager->getStatus(_project.getID()).weekTotal.asMillis();
        case COL_MONTH:
            return _statusManager->getStatus(_project.getID()).monthTotal.asMillis();
        case COL_TOTAL:
            return _statusManager->getStatus(_project.getID()).allTotal.asMillis();
        default:
            return 0;
    }
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
        case COL_TOTAL:
            return _statusManager->getStatus(_project.getID()).allTotal.formatShort();
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

void ProjectTreeItem::insertChild(ProjectTreeItem *child, int index) {
    _childItems.insert(index, child);
    child->_parentItem = this;
}

bool ProjectTreeItem::removeChildAt(int index) {
    if (index < 0 || index >= _childItems.size()) {
        return false;
    }
    _childItems.removeAt(index);
    return true;
}

void ProjectTreeItem::reset() {
    qDeleteAll<QList<ProjectTreeItem *>>(_childItems);
    _childItems.clear();
}
