#ifndef TOM_UI_PROJECTLISTMODEL_H
#define TOM_UI_PROJECTLISTMODEL_H

#include <QtCore/QAbstractListModel>

#include "source/gotime/TomControl.h"

class ProjectListModel : public QAbstractListModel {
Q_OBJECT

public:
    ProjectListModel(TomControl *control, QObject *parent);

    QVariant data(const QModelIndex &index, int role) const override;

    int rowCount(const QModelIndex &parent) const override;

private:
    TomControl *_control;
    QList<Project> _projects;
};


#endif //TOM_UI_PROJECTLISTMODEL_H
