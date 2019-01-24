#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QBrush>
#include <icons.h>

#include "gotime/GotimeControl.h"
#include "ProjectTreeModel.h"
#include "UserRoles.h"

ProjectTreeModel::ProjectTreeModel(GotimeControl *control, ProjectStatusManager *statusManager, QObject *parent) : QAbstractItemModel(parent),
                                                                                                                   _control(control),
                                                                                                                   _statusManager(statusManager) {
    _headers = QStringList() << "Name" << "Today" << "This week" << "This month" << "Total";
    loadProjects();

    connect(_control, &GotimeControl::projectCreated, this, &ProjectTreeModel::addProject);
    connect(_control, &GotimeControl::dataResetNeeded, this, &ProjectTreeModel::loadProjects);
}

ProjectTreeModel::~ProjectTreeModel() {
    delete _rootItem;
}

void ProjectTreeModel::loadProjects() {
    qDebug() << "loadProject (reset)";

    beginResetModel();

    if (_rootItem) {
        //fixme cleanup
//        delete _rootItem;
        _rootItem = nullptr;
    }

    _projects = _control->loadProjects();

    _visibleRootItem = new ProjectTreeRootItem(_statusManager, _rootItem);
    addProjectItems(_projects, _visibleRootItem);
    _rootItem = new ProjectTreeRootItem(_statusManager);
    _rootItem->appendChild(_visibleRootItem);

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

    if (role == Qt::DecorationRole) {
        if (index.column() == ProjectTreeItem::COL_NAME) {
            auto *item = static_cast<ProjectTreeItem *>(index.internalPointer());
            if (_control->isStarted(item->getProject())) {
                return Icons::stopTimer();
            }
        }
    }

    if (role == IDRole) {
        auto *item = static_cast<ProjectTreeItem *>(index.internalPointer());
        return item->getProject().getID();
    }

    if (role == SortValueRole) {
        auto *item = static_cast<ProjectTreeItem *>(index.internalPointer());
        return item->sortData(index.column());
    }

    return QVariant();
}

bool ProjectTreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    qDebug() << "setData" << index << value << role;

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
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }

    // the visible root is not draggable, but accepts drops
    if (item == _visibleRootItem) {
        return Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);
    }
    return Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);
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
    const QModelIndexList &list = match(index(0, 0, QModelIndex()), IDRole, projectID, 1, Qt::MatchExactly | Qt::MatchRecursive);
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

Project ProjectTreeModel::projectAtIndex(const QModelIndex &index) {
    void *pointer = index.internalPointer();
    if (pointer == nullptr) {
        return Project();
    }

    auto *item = static_cast<ProjectTreeItem *>(pointer);
    return item->getProject();
}

ProjectTreeItem *ProjectTreeModel::projectItemAtIndex(const QModelIndex &index) {
    void *pointer = index.internalPointer();
    if (pointer == nullptr) {
        return nullptr;
    }

    return static_cast<ProjectTreeItem *>(pointer);
}

void ProjectTreeModel::addProject(const Project &project) {
    if (!project.isValid()) {
        return;
    }

    const QModelIndex &row = getProjectRow(project.getID());
    if (row.isValid()) {
        return;
    }

    const QList<Project> projects = QList<Project>() << project;

    QModelIndex parentRow;

    if (project.getParentID().isEmpty()) {
        parentRow = index(0, 0, QModelIndex());
    } else {
        parentRow = getProjectRow(project.getParentID());
    }

    ProjectTreeItem *parentItem = projectItemAtIndex(parentRow);

    beginInsertRows(parentRow, parentItem->childCount(), parentItem->childCount());
    parentItem->appendChild(new ProjectTreeItem(project, _statusManager, parentItem));
    endInsertRows();
}

Qt::DropActions ProjectTreeModel::supportedDropActions() const {
    return Qt::MoveAction;
}

QStringList ProjectTreeModel::mimeTypes() const {
    return QStringList() << PROJECT_MIME_TYPE;
}

QMimeData *ProjectTreeModel::mimeData(const QModelIndexList &indexes) const {
    QStringList projectIDs;
    for (auto i : indexes) {
        ProjectTreeItem *treeItem = getItem(i);
        if (treeItem == nullptr || !treeItem->getProject().isValid()) {
            return nullptr;
        }
        projectIDs << treeItem->getProject().getID();
    }

    auto *result = new QMimeData();
    result->setData(PROJECT_MIME_TYPE, projectIDs.join("||").toUtf8());
    return result;
}


bool ProjectTreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
    if (!parent.isValid() || action != Qt::MoveAction) {
        return false;
    }

    if (!data->hasFormat(PROJECT_MIME_TYPE)) {
        return false;
    }

    const QByteArray &bytes = data->data(PROJECT_MIME_TYPE);
    if (bytes.isEmpty()) {
        return false;
    }

    const QStringList ids = QString::fromUtf8(bytes).split("||", QString::SkipEmptyParts);
    if (ids.isEmpty()) {
        return false;
    }

    ProjectTreeItem *parentItem = getItem(parent);
    if (!parentItem) {
        return false;
    }

    const QString &parentID = parentItem->getProject().getID();

    // don't move data in the model if the data couldn't be changed in tom
    bool success = _control->updateProjects(ids, false, "", true, parentID);
    if (!success) {
        qDebug() << "tom update failed for move of projects" << ids << "into" << parentID;
        return false;
    }

    for (const auto &projectID : ids) {
        const QModelIndex &rowIndex = getProjectRow(projectID);
        if (!rowIndex.isValid()) {
            return false;
        }

        // using moveRows here breaks our proxy model. The View class seems to remove the source rows on its own when move/moveInternal is used
        // therefore we're just inserting and let the View call removeRows()
        beginInsertRows(parent, 0, 0);
        parentItem->insertChild(getItem(rowIndex), 0);
        endInsertRows();
    }

    return true;
}

bool ProjectTreeModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) {
    qDebug() << "moveRows: from parent" << getItem(sourceParent)->getProject().getName() << ", row" << sourceRow << "(count" << count << ") to parent"
             << getItem(destinationParent)->getProject().getName() << "at row" << destinationChild << "";

    // fixme make sure that source is not moved onto a child element
    // fixme fix collapsing tree if proxy model is used (use removeRow insertRow instead?)

    bool ok = beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, 0);
    if (!ok) {
        qDebug() << "beginMoveRows returned false";
        return false;
    }

    ProjectTreeItem *oldParent = getItem(sourceParent);
    ProjectTreeItem *newParent = getItem(destinationParent);
    for (int i = 0; i < count; i++) {
        const QModelIndex &sourceRowIndex = sourceParent.child(sourceRow + i, 0);
        ProjectTreeItem *rowItem = getItem(sourceRowIndex);
        if (!rowItem) {
            qDebug() << "source row invalid row found" << i;
            continue;
        }

        qDebug() << "moving item" << rowItem->getProject().getName() << "into parent" << newParent->getProject().getName();
        oldParent->removeChildAt(sourceRow + i);
        newParent->insertChild(rowItem, 0);
    }

    endMoveRows();

    return true;
}

bool ProjectTreeModel::insertRows(int row, int count, const QModelIndex &parent) {
    ProjectTreeItem *parentItem = getItem(parent);
    qDebug() << "inserting rows at" << row << "count" << count << "into" << parentItem->getProject().getName();

    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; i++) {
        parentItem->insertChild(new ProjectTreeItem(Project(), _statusManager, parentItem), i);
    }
    endInsertRows();

    return true;
}

bool ProjectTreeModel::removeRows(int row, int count, const QModelIndex &parent) {
    qDebug() << "removing rows" << row << "to" << row + count - 1 << "from" << getItem(parent)->getProject().getName();
    if (!parent.isValid()) {
        return false;
    }

    ProjectTreeItem *parentItem = getItem(parent);
    bool result = true;

    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; i++) {
        if (!parentItem->removeChildAt(row + i)) {
            qDebug() << "removing item failed" << i;
            result = false;
            break;
        }
    }
    endRemoveRows();

    return result;
}
