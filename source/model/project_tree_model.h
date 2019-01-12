#ifndef GOTIME_UI_PROJECTTREEMODEL_H
#define GOTIME_UI_PROJECTTREEMODEL_H


#include <QtCore/QAbstractItemModel>

#include "data/Project.h"
#include "gotime/gotime_control.h"
#include "project_tree_item.h"

class project_tree_model : public QAbstractItemModel {
Q_OBJECT

public:
    project_tree_model(gotime_control *control, QObject *parent);

    ~project_tree_model() override;

    QVariant data(const QModelIndex &index, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;

    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent) const override;

    int columnCount(const QModelIndex &parent) const override;

private:
    ProjectTreeItem *_rootItem;
    gotime_control *_control;
    QList<Project> _projects;
    ProjectsStatus _status;

    ProjectTreeItem *createModelItem(const QList<Project> &allProjects, const Project &project, ProjectTreeItem* parent);

    void refreshProjects(ProjectTreeItem* root);
    void printProjects(int level, ProjectTreeItem* root);
};

#endif //GOTIME_UI_PROJECTTREEMODEL_H
