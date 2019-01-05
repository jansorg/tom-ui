#include <QStringList>
#include <QtCore>

#include "project_tree_item.h"

ProjectTreeItem::ProjectTreeItem(const QList<QVariant> &data, ProjectTreeItem *parent) : m_itemData(data),
                                                                                               m_parentItem(parent) {
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
    return m_itemData.count();
}

QVariant ProjectTreeItem::data(int column) const {
    const QVariant &value = m_itemData.value(column);
    qDebug() << "data" << value;
    return value;
}

ProjectTreeItem * ProjectTreeItem::parentItem() {
    return m_parentItem;
}

int ProjectTreeItem::row() const {
    if (m_parentItem){
        return m_parentItem->m_childItems.indexOf(const_cast<ProjectTreeItem *>(this));
    }

    return 0;
}