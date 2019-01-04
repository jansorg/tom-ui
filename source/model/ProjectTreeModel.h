#ifndef GOTIME_UI_PROJECTTREEMODEL_H
#define GOTIME_UI_PROJECTTREEMODEL_H


#include <QtCore/QAbstractItemModel>
#include <data/Project.h>

class ProjectTreeModel : public QAbstractItemModel {
Q_OBJECT

public:
    ProjectTreeModel(QList<Project *> &projects, QObject *parent);

private:
//    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;

private:
    QModelIndex parent(const QModelIndex &child) const override;

private:
    QList<Project *> projects;

    int rowCount(const QModelIndex &parent) const override;

    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;
};

#endif //GOTIME_UI_PROJECTTREEMODEL_H
