#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <data/Project.h>
#include <gotime/ProjectStatus.h>
#include <gotime/ProjectStatusManager.h>

class ProjectTreeItem {
public:
    explicit ProjectTreeItem(const Project &project, const ProjectStatusManager *statusManager, ProjectTreeItem *parentItem = nullptr);

    ~ProjectTreeItem();

    void appendChild(ProjectTreeItem *child);

    void insertChild(ProjectTreeItem *child, int atIndex);

    void removeChild(ProjectTreeItem *child);

    bool removeChildAt(int index);

    void reset();

    ProjectTreeItem *childAt(int row);

    int childCount() const;

    QVariant sortData(int column) const;

    virtual QVariant data(int column) const;

    virtual bool setData(int column, const QVariant &value);

    int row() const;

    virtual ProjectTreeItem *parentItem();

    const Project &getProject() const;

    virtual void refreshWith(const Project &project);

public:
    static const int COL_NAME = 0;
    static const int COL_TODAY = 1;
    static const int COL_YESTERDAY = 2;
    static const int COL_WEEK = 3;
    static const int COL_MONTH = 4;
    static const int COL_YEAR = 5;
    static const int COL_TOTAL = 6;

    static const int FIRST_STATUS_COL_INDEX = COL_TODAY;
    static const int LAST_COL_INDEX = COL_TOTAL;
    static const int COL_COUNT = LAST_COL_INDEX + 1;

protected:
    Project _project;
    const ProjectStatusManager *_statusManager;
    ProjectTreeItem *_parentItem;

    QList<ProjectTreeItem *> _childItems;
    QString _projectName;
};

#endif // TREEITEM_H