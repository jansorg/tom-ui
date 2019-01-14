#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <data/Project.h>

class ProjectTreeItem {
public:
    explicit ProjectTreeItem(const QList<QVariant> &data, const Project &project, ProjectTreeItem *parentItem = 0);

    ~ProjectTreeItem();

    void appendChild(ProjectTreeItem *child);

    ProjectTreeItem *child(int row);

    int childCount() const;

    int columnCount() const;

    QVariant data(int column) const;

    bool setData(int column, const QVariant &value);

    int row() const;

    ProjectTreeItem *parentItem();

    const Project &getProject() const;

public:
    static const int COL_NAME = 0;
    static const int COL_DAY = 1;
    static const int COL_WEEK = 2;
    static const int COL_MONTH = 3;

private:
    QList<ProjectTreeItem *> m_childItems;
    QList<QVariant> _itemData;
    ProjectTreeItem *m_parentItem;
    const Project &_project;
};

#endif // TREEITEM_H