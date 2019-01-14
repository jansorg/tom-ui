#include <QStringList>
#include <QtCore>

#include "project_tree_item.h"

ProjectTreeItem::ProjectTreeItem(const QList<QVariant> &data, const Project &project, ProjectTreeItem *parent)
        : _itemData(data), m_parentItem(parent), _project(project) {

}

ProjectTreeItem::~ProjectTreeItem() {
    qDeleteAll(m_childItems);
}

void ProjectTreeItem::appendChild(ProjectTreeItem *item) {
    m_childItems.append(item);
}

ProjectTreeItem *ProjectTreeItem::child(int row) {
    return m_childItems.value(row);
}

int ProjectTreeItem::childCount() const {
    return m_childItems.count();
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
    return m_parentItem;
}

int ProjectTreeItem::row() const {
    if (m_parentItem) {
        return m_parentItem->m_childItems.indexOf(const_cast<ProjectTreeItem *>(this));
    }

    return 0;
}

const Project &ProjectTreeItem::getProject() const {
    return _project;
}
