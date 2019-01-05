#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>

class ProjectTreeItem {
public:
    explicit ProjectTreeItem(const QList<QVariant> &data, ProjectTreeItem *parentItem = 0);

    ~ProjectTreeItem();

    void appendChild(ProjectTreeItem *child);

    ProjectTreeItem *child(int row);

    int childCount() const;

    int columnCount() const;

    QVariant data(int column) const;

    int row() const;

    ProjectTreeItem * parentItem();

private:
    QList<ProjectTreeItem *> m_childItems;
    QList<QVariant> m_itemData;
    ProjectTreeItem *m_parentItem;
};

#endif // TREEITEM_H