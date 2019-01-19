#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QBrush>

#include "gotime/GotimeControl.h"
#include "ProjectTreeModel.h"

enum ProjectRoles {
    idRole = Qt::UserRole
};

ProjectTreeModel::ProjectTreeModel(GotimeControl *control, ProjectStatusManager *statusManager, QObject *parent) : QAbstractItemModel(parent),
                                                                                                                   _control(control),
                                                                                                                   _statusManager(statusManager) {
    _headers = QStringList() << "Name" << "Today" << "This week" << "This month" << "Total";
    loadProjects();
//    printProjects(0, _rootItem);
}

ProjectTreeModel::~ProjectTreeModel() {
    delete _rootItem;
}

void ProjectTreeModel::loadProjects() {
    beginResetModel();

    if (_rootItem) {
//        delete _rootItem;
        _rootItem = nullptr;
    }

    _projects = _control->loadProjects();

    auto *visibleRoot = new ProjectTreeRootItem(_statusManager, _rootItem);
    addProjectItems(_projects, visibleRoot);
    _rootItem = new ProjectTreeRootItem(_statusManager);
    _rootItem->appendChild(visibleRoot);

    endResetModel();
}

void ProjectTreeModel::addProjectItems(const QList<Project> &allProjects, ProjectTreeItem *parent) {
    const auto &parentID = parent->getProject().getID();

    for (const auto &p: allProjects) {
        if (p.getParentID() == parentID) {
            auto *item = new ProjectTreeItem(p, _statusManager, parent);
            parent->appendChild(item);

            addProjectItems(allProjects, item);
        }
    }
}

QVariant ProjectTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::TextAlignmentRole && index.column() >= ProjectTreeItem::COL_DAY) {
        return Qt::AlignRight + Qt::AlignVCenter;
    }

    if (role == Qt::ForegroundRole) {
        auto *item = static_cast<ProjectTreeItem *>(index.internalPointer());
        if (index.column() >= ProjectTreeItem::COL_DAY && item->data(index.column()).toString() == QString("0:00h")) {
            return QVariant(QColor(Qt::gray));
        };
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        auto *item = static_cast<ProjectTreeItem *>(index.internalPointer());
        return item->data(index.column());
    }

//    if (role == Qt::DecorationRole && index.column() == ProjectTreeItem::COL_NAME) {
//        auto *item = static_cast<ProjectTreeItem *>(index.internalPointer());
//        if (_control->isStarted(item->getProject())) {
//            return QVariant(QColor(Qt::green));
//        }
//    }

    if (role == Qt::BackgroundColorRole) {
        auto *item = static_cast<ProjectTreeItem *>(index.internalPointer());
        if (_control->isStarted(item->getProject())) {
            return QBrush(QColor(255, 255, 210));
        }
    }

    if (role == idRole) {
        auto *item = static_cast<ProjectTreeItem *>(index.internalPointer());
        return item->getProject().getID();
    }

    return QVariant();
}

bool ProjectTreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid() || role != Qt::EditRole) {
        return false;
    }

    ProjectTreeItem *item = getItem(index);
    if (!item || item == _rootItem) {
        return false;
    }

    if (index.column() == 0) {
        QString newName = value.toString();
        bool ok = _control->renameProject(item->getProject().getID(), newName);
        if (ok && item->setData(index.column(), newName)) {
            emit dataChanged(index, index);
        }
        return ok;
    }

    return false;
}

Qt::ItemFlags ProjectTreeModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    ProjectTreeItem *item = getItem(index);
    if (item && item->getProject().isValid() && index.column() == ProjectTreeItem::COL_NAME) {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }

    return QAbstractItemModel::flags(index);
}

QVariant ProjectTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        return _headers.at(section);
    }

    if (role == Qt::TextAlignmentRole) {
        if (section >= ProjectTreeItem::FIRST_STATUS_COL_INDEX) {
            return Qt::AlignRight;
        }
        return Qt::AlignLeft;
    }

    return QVariant();
}

QModelIndex ProjectTreeModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent)) {
        return {};
    }

    ProjectTreeItem *parentItem = getItem(parent);
    if (!parentItem) {
        return {};
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

    auto *childItem = getItem(index);
    if (!childItem || childItem == _rootItem) {
        return {};
    }

    auto *parentItem = childItem->parentItem();
    if (!parentItem) {
        return {};
    }
    return createIndex(parentItem->row(), 0, parentItem);
}

int ProjectTreeModel::rowCount(const QModelIndex &parent) const {
    if (parent.column() > ProjectTreeItem::COL_NAME) {
        return 0;
    }

    ProjectTreeItem *parentItem = getItem(parent);
    return parentItem->childCount();
}

ProjectTreeItem *ProjectTreeModel::getItem(const QModelIndex &index) const {
    if (index.isValid()) {
        auto *item = static_cast<ProjectTreeItem *>(index.internalPointer());
        if (item) {
            return item;
        }
    }
    return _rootItem;
}

void ProjectTreeModel::printProjects(int level, ProjectTreeItem *root) {
    qDebug() << QString(" ").repeated(level) << root->data(0).toString();
    for (int i = 0; i < root->childCount(); ++i) {
        printProjects(level + 1, root->child(i));
    }
}

QModelIndex ProjectTreeModel::getProjectRow(const QString &projectID) const {
    const QModelIndexList &list = match(index(0, 0, QModelIndex()), idRole, projectID, 1, Qt::MatchExactly | Qt::MatchRecursive);
    if (list.size() == 1) {
        return list.first();
    }
    return {};
}

void ProjectTreeModel::updateProject(const Project &project) {
    const QModelIndex &row = getProjectRow(project.getID());
    if (row.isValid()) {
        ProjectTreeItem *item = getItem(row);
        if (item) {
            item->refreshWith(project);
            emit dataChanged(row, row.siblingAtColumn(ProjectTreeItem::LAST_COL_INDEX));
        }
    }
}

void ProjectTreeModel::updateProjectStatus(const QString &projectID) {
    const QModelIndex &row = getProjectRow(projectID);
    if (row.isValid()) {
        ProjectTreeItem *item = getItem(row);
        if (item) {
            emit dataChanged(row.siblingAtColumn(ProjectTreeItem::FIRST_STATUS_COL_INDEX), row.siblingAtColumn(ProjectTreeItem::LAST_COL_INDEX));
        }
    }
}

