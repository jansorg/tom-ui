#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <data/Project.h>
#include <gotime/projectstatus.h>
#include <gotime/projectstatusmanager.h>

class ProjectTreeItem {
public:
    explicit ProjectTreeItem(const Project &project, const ProjectStatusManager *statusManager, ProjectTreeItem *parentItem = nullptr);

    ~ProjectTreeItem();

    void appendChild(ProjectTreeItem *child);

    ProjectTreeItem *child(int row);

    int childCount() const;

    virtual QVariant data(int column) const;

    virtual bool setData(int column, const QVariant &value);

    int row() const;

    virtual ProjectTreeItem *parentItem();

    const Project &getProject() const;

    virtual void refreshWith(const Project &project);

public:
    static const int COL_NAME = 0;
    static const int COL_DAY = 1;
    static const int COL_WEEK = 2;
    static const int COL_MONTH = 3;
    static const int COL_TOTAL = 4;

    static const int FIRST_STATUS_COL_INDEX = COL_DAY;
    static const int LAST_COL_INDEX = COL_TOTAL;
    static const int COL_COUNT = LAST_COL_INDEX + 1;

private:
    Project _project;
    const ProjectStatusManager *_statusManager;
    ProjectTreeItem *_parentItem;

    QList<ProjectTreeItem *> _childItems;
    QString _projectName;
};

#endif // TREEITEM_H