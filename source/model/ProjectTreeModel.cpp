#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QBrush>
#include <QtGui/QList>
#include <icons.h>

#include "gotime/TomControl.h"
#include "ProjectTreeModel.h"
#include "UserRoles.h"
#include "FrameTableViewModel.h"

ProjectTreeModel::ProjectTreeModel(TomControl *control, ProjectStatusManager *statusManager, QObject *parent) : QAbstractItemModel(parent),
                                                                                                                _control(control),
                                                                                                                _statusManager(statusManager),
                                                                                                                _rootItem(nullptr),
                                                                                                                _visibleRootItem(nullptr) {
    _headers = QStringList() << "Name" << "Today" << "This week" << "This month" << "Total";

    if (!_rootItem) {
        _rootItem = new ProjectTreeRootItem(_statusManager);
        _visibleRootItem = new ProjectTreeRootItem(_statusManager, _rootItem);
        _rootItem->appendChild(_visibleRootItem);
    } else {
        _visibleRootItem->reset();
    }

    _projects = _control->loadProjects();
    addProjectItems(_projects, _visibleRootItem);

    connect(_control, &TomControl::projectCreated, this, &ProjectTreeModel::addProject);
    connect(_control, &TomControl::dataResetNeeded, this, &ProjectTreeModel::loadProjects);
}

ProjectTreeModel::~ProjectTreeModel() {
    delete _rootItem;
}

void ProjectTreeModel::loadProjects() {
    qDebug() << "loadProject (reset)";

    beginResetModel();
    _visibleRootItem->reset();
    _projects = _control->loadProjects();
    addProjectItems(_projects, _visibleRootItem);
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
        return Qt::AlignTrailing + Qt::AlignVCenter;
    }

    if (role == Qt::ForegroundRole) {
        auto *item = getItem(index);
        if (index.column() >= ProjectTreeItem::COL_DAY && item->data(index.column()).toString() == QString("0:00h")) {
            return QVariant(QColor(Qt::gray));
        };
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return getItem(index)->data(index.column());
    }

    if (role == Qt::DecorationRole) {
        if (index.column() == ProjectTreeItem::COL_NAME && _control->isStarted(getItem(index)->getProject())) {
            return Icons::stopTimer();
        }
    }

    if (role == IDRole) {
        return getItem(index)->getProject().getID();
    }

    if (role == SortValueRole && index.column() < ProjectTreeItem::FIRST_STATUS_COL_INDEX) {
        return getItem(index)->data(index.column());
    } else if (role == SortValueRole) {
        return getItem(index)->sortData(index.column());
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
            return Qt::AlignTrailing;
        }
        return Qt::AlignLeading;
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
    return QStringList() << PROJECTS_MIME_TYPE << FRAMES_MIME_TYPE;
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
    result->setData(PROJECTS_MIME_TYPE, projectIDs.join("||").toUtf8());
    return result;
}

bool ProjectTreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
    if (data->hasFormat(PROJECTS_MIME_TYPE)) {
        return handleDropProjectIDs(data, action, row, column, parent);
    } else if (data->hasFormat(FRAMES_MIME_TYPE)) {
        return handleDropFrameIDs(data, action, row, column, parent);
    }
    return false;
}

bool ProjectTreeModel::handleDropProjectIDs(const QMimeData *data, Qt::DropAction action, int /*row*/, int /*column*/, const QModelIndex &parent) {
    if (action != Qt::MoveAction) {
        return false;
    }

    const QByteArray &bytes = data->data(PROJECTS_MIME_TYPE);
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

bool ProjectTreeModel::handleDropFrameIDs(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
    if (!parent.isValid() || action != Qt::MoveAction) {
        return false;
    }

    const QByteArray &bytes = data->data(FRAMES_MIME_TYPE);
    if (bytes.isEmpty()) {
        return false;
    }

    QStringList ids = QString::fromUtf8(bytes).split("||", QString::SkipEmptyParts);
    if (ids.isEmpty()) {
        return false;
    }

    // first item is the id of the source project
    const QString &sourceProjectID = ids.takeFirst();

    ProjectTreeItem *parentItem = getItem(parent);
    if (!parentItem || parentItem == _visibleRootItem || !parentItem->getProject().isValid()) {
        return false;
    }

    const QString &parentProjectID = parentItem->getProject().getID();

    // don't move data in the model if the data couldn't be changed in tom
    bool success = _control->updateFrame(ids, sourceProjectID, false, QDateTime(), false, QDateTime(), false, "", true, parentProjectID);
    if (!success) {
        qDebug() << "tom update failed for move of frames" << ids << "into" << parentProjectID;
        return false;
    }
    return true;
}

/*
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
*/

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
