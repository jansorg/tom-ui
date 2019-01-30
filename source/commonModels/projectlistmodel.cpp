#include "source/icons.h"
#include "source/model/UserRoles.h"
#include "projectlistmodel.h"

ProjectListModel::ProjectListModel(TomControl *control, QObject *parent) : QAbstractListModel(parent), _control(control) {
    _projects = _control->loadProjects();
}

QVariant ProjectListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    Project project = _projects.at(index.row());
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return project.getName();
    }

    if (role == UserRoles::IDRole) {
        return project.getID();
    }

    if (role == Qt::DecorationRole) {
        return Icons::projectStart();
    }

    return QVariant();
}

int ProjectListModel::rowCount(const QModelIndex &parent) const {
    if (!parent.isValid()) {
        return _projects.size();
    }
    return 0;
}
