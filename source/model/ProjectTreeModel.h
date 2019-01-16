#ifndef GOTIME_UI_PROJECTTREEMODEL_H
#define GOTIME_UI_PROJECTTREEMODEL_H


#include <QtCore/QAbstractItemModel>

#include "data/Project.h"
#include "gotime/GotimeControl.h"
#include "project_tree_item.h"

class ProjectTreeModel : public QAbstractItemModel {
Q_OBJECT

public:
    ProjectTreeModel(GotimeControl *control, QObject *parent);

    ~ProjectTreeModel() override;

    QVariant data(const QModelIndex &index, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;

    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent) const override;

    int columnCount(const QModelIndex &parent) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    ProjectTreeItem *getItem(const QModelIndex &index) const;

private:
    ProjectTreeItem *_rootItem;
    GotimeControl *_control;
    QList<Project> _projects;
    ProjectsStatus _status;

    ProjectTreeItem *createModelItem(const QList<Project> &allProjects, const Project &project, ProjectTreeItem* parent);

    void refreshProjects(ProjectTreeItem* root);
    void printProjects(int level, ProjectTreeItem* root);
};

#endif //GOTIME_UI_PROJECTTREEMODEL_H
