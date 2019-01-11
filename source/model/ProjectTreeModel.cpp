#include <gotime/GotimeControl.h>
#include "ProjectTreeModel.h"

ProjectTreeModel::ProjectTreeModel(GotimeControl *control, QObject *parent) : _control(control),
                                                                              QAbstractItemModel(parent) {
    QList<QVariant> headers;
    headers << "Name" << "Year" << "Month" << "Week" << "Day";

    _rootItem = new ProjectTreeItem(headers, Project());

    _projects.clear();
    _projects << _control->loadProjects();

    refreshProjects(_rootItem);
    printProjects(0, _rootItem);
}

ProjectTreeModel::~ProjectTreeModel() {
    delete _rootItem;
}

void ProjectTreeModel::refreshProjects(ProjectTreeItem *root) {
    for (const auto &project : _projects) {
        if (project.getParentID().isEmpty()) {
            root->appendChild(createModelItem(_projects, project, root));
        }
    }
}

ProjectTreeItem *
ProjectTreeModel::createModelItem(const QList<Project> &allProjects, const Project &project, ProjectTreeItem *parent) {
    QList<QVariant> items;
    items << project.getShortName()
          << project.trackedYear().format()
          << project.trackedMonth().format()
          << project.trackedWeek().format()
          << project.trackedDay().format();

    auto *item = new ProjectTreeItem(items, project, parent);
    for (const auto &p: allProjects) {
        if (p.getParentID() == project.getID()) {
            item->appendChild(createModelItem(allProjects, p, item));
        }
    }

    return item;
}

int ProjectTreeModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return static_cast<ProjectTreeItem *>(parent.internalPointer())->columnCount();
    } else {
        return _rootItem->columnCount();
    }
}

QVariant ProjectTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    auto *item = static_cast<ProjectTreeItem *>(index.internalPointer());
    return item->data(index.column());
}

Qt::ItemFlags ProjectTreeModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return 0;
    }

    return QAbstractItemModel::flags(index);
}

QVariant ProjectTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return _rootItem->data(section);
    }

    return QVariant();
}

QModelIndex ProjectTreeModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent)) {
        return {};
    }

    ProjectTreeItem *parentItem;
    if (!parent.isValid()) {
        parentItem = _rootItem;
    } else {
        parentItem = static_cast<ProjectTreeItem *>(parent.internalPointer());
    }

    ProjectTreeItem *childItem = parentItem->child(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    }
    return {};
}

QModelIndex ProjectTreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid()) {
        return {};
    }

    auto *childItem = static_cast<ProjectTreeItem *>(index.internalPointer());
    auto *parentItem = childItem->parentItem();

    if (parentItem == _rootItem) {
        return {};
    }
    return createIndex(parentItem->row(), 0, parentItem);
}

int ProjectTreeModel::rowCount(const QModelIndex &parent) const {
    if (parent.column() > 0) {
        return 0;
    }

    ProjectTreeItem *parentItem;
    if (!parent.isValid()) {
        parentItem = _rootItem;
    } else {
        parentItem = static_cast<ProjectTreeItem *>(parent.internalPointer());
    }
    return parentItem->childCount();
}

void ProjectTreeModel::printProjects(int level, ProjectTreeItem *root) {
    qDebug() << QString(" ").repeated(level) << root->data(0);
    for (int i = 0; i < root->childCount(); ++i) {
        printProjects(level + 1, root->child(i));
    }
}

