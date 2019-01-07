#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <data/Project.h>

class ProjectTreeItem {
public:
    explicit ProjectTreeItem(const QList<QVariant> &data, const Project& project, ProjectTreeItem *parentItem = 0);

    ~ProjectTreeItem();

    void appendChild(ProjectTreeItem *child);

    ProjectTreeItem *child(int row);

    int childCount() const;

    int columnCount() const;

    QVariant data(int column) const;

    int row() const;

    ProjectTreeItem *parentItem();

    const Project& getProject() const;

private:
    QList<ProjectTreeItem *> m_childItems;
    QList<QVariant> m_itemData;
    ProjectTreeItem *m_parentItem;
    const Project& _project;
};

#endif // TREEITEM_H