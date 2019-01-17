#include <QStringList>
#include <QtCore>

#include "project_tree_item.h"

ProjectTreeItem::ProjectTreeItem(const QList<QVariant> &data) : _parentItem(nullptr),
                                                                _project(Project()),
                                                                _itemData(data) {}

ProjectTreeItem::ProjectTreeItem(const Project &project, const ProjectStatus &status, ProjectTreeItem *parent)
        : _parentItem(parent), _project(project) {

    refreshWith(project, status);
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

int ProjectTreeItem::columnCount() const {
    return _itemData.count();
}

QVariant ProjectTreeItem::data(int column) const {
    return _itemData.value(column);
}

bool ProjectTreeItem::setData(int column, const QVariant &value) {
    if (column == COL_NAME) {
        _itemData[column] = value;
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

void ProjectTreeItem::refreshWith(const Project &project, const ProjectStatus &status) {
    _itemData = QList<QVariant>() << project.getShortName()
                                  << status.dayTotal.formatShort()
                                  << status.weekTotal.formatShort()
                                  << status.monthTotal.formatShort();
}
