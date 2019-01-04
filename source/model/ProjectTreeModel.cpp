//
// Created by jansorg on 04.01.19.
//

#include "ProjectTreeModel.h"

ProjectTreeModel::ProjectTreeModel(QList<Project> &projects, QObject *parent) : QAbstractItemModel(parent) {
    this->projects = projects;
}

int ProjectTreeModel::rowCount(const QModelIndex &parent) const {
    return projects.size();
}

int ProjectTreeModel::columnCount(const QModelIndex &parent) const {
    return 2;
}

QVariant ProjectTreeModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        const QString &string = projects.at(index.row()).getName();
        return QVariant(string);
    }
    return QVariant();
}

QModelIndex ProjectTreeModel::parent(const QModelIndex &child) const {
    return {};
}

QModelIndex ProjectTreeModel::index(int row, int column, const QModelIndex &parent) const {
    return createIndex(row, column);
}

