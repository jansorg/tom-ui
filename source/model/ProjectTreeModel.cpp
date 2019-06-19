#include <QtWidgets/QApplication>

#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QBrush>
#include <QtGui/QList>
#include <QtGui/QPalette>

#include "gotime/TomControl.h"
#include "ProjectTreeModel.h"
#include "UserRoles.h"
#include "FrameTableViewModel.h"
#include "icons.h"

ProjectTreeModel::ProjectTreeModel(TomControl *control, ProjectStatusManager *statusManager, bool showOverallProject,
                                   QObject *parent, bool enableUpdates, bool enableCheckboxes)
        : QAbstractItemModel(parent),
          _control(control),
          _statusManager(statusManager),
          _rootItem(nullptr),
          _visibleRootItem(nullptr),
          _headers(QStringList() << tr("Name") << tr("Today") << tr("Yesterday") << tr("This week") << tr("This month") << tr("This year") << tr("Total")),
          _enableCheckboxes(enableCheckboxes) {

    _rootItem = new ProjectTreeRootItem(_statusManager);
    if (showOverallProject) {
        _visibleRootItem = new ProjectTreeRootItem(_statusManager, _rootItem);
        _rootItem->appendChild(_visibleRootItem);
    } else {
        _visibleRootItem = _rootItem;
    }

    if (enableUpdates) {
        connect(_control, &TomControl::projectCreated, this, &ProjectTreeModel::addProject);
        connect(_control, &TomControl::projectRemoved, this, &ProjectTreeModel::removeProject);
        connect(_control, &TomControl::projectHierarchyChanged, this, &ProjectTreeModel::onProjectHierarchyChange);
        connect(_control, &TomControl::dataResetNeeded, this, &ProjectTreeModel::loadProjects);
    }
}

ProjectTreeModel::~ProjectTreeModel() {
    delete _rootItem;
}

void ProjectTreeModel::loadProjects() {
    beginResetModel();

    _visibleRootItem->reset();
    _projects = _control->loadProjects();
    setupItem(_visibleRootItem, _projects);

    endResetModel();
}

void ProjectTreeModel::setupItem(ProjectTreeItem *parent, QList<Project> &projects) {
    const auto &parentProject = parent->getProject();
    for (const auto &p: projects) {
        if (p.getParentID() == parentProject.getID()) {
            auto *item = new ProjectTreeItem(p, _statusManager, parent);
            parent->appendChild(item);

            setupItem(item, projects);
        }
    }
}

QVariant ProjectTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::TextAlignmentRole && index.column() >= ProjectTreeItem::FIRST_STATUS_COL_INDEX) {
        return Qt::AlignTrailing + Qt::AlignVCenter;
    }

    if (role == Qt::ForegroundRole) {
        auto *item = projectItem(index);

        if (index.column() == ProjectTreeItem::COL_TODAY && _control->isStarted(item->getProject(), false)) {
            return QColor(Qt::red);
        }

        if (index.column() >= ProjectTreeItem::COL_TODAY && item->data(index.column()).toString() == "0:00h"){
            return QColor(Qt::lightGray);
        }
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return projectItem(index)->data(index.column());
    }

    if (role == Qt::DecorationRole && index.column() == ProjectTreeItem::COL_TODAY) {
        if (_control->isStarted(projectItem(index)->getProject())) {
            return Icons::activeProject();
        }

        auto *item = projectItem(index);
        if ((item == _visibleRootItem && _control->cachedActiveProject().isValid()) ||
            _control->isStarted(item->getProject(), true)) {
            return Icons::withActiveSubproject();
        }
    }

    if (_enableCheckboxes && role == Qt::CheckStateRole && index.column() == ProjectTreeItem::COL_NAME) {
        const QString &id = projectItem(index)->getProject().getID();
        return _checkedProjectIDs.contains(id) ? Qt::Checked : Qt::Unchecked;
    }

    if (role == IDRole) {
        return projectItem(index)->getProject().getID();
    }

    if (role == SortValueRole) {
        return projectItem(index)->sortData(index.column());
    }

    return QVariant();
}

bool ProjectTreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    qDebug() << "setData" << index << value << role;

    if (_enableCheckboxes && role == Qt::CheckStateRole && index.column() == ProjectTreeItem::COL_NAME) {
        const QString &id = projectItem(index)->getProject().getID();
        if (value == Qt::Checked) {
            _checkedProjectIDs.insert(id);
        } else {
            _checkedProjectIDs.remove(id);
        }
        return true;
    }

    if (!index.isValid() || role != Qt::EditRole) {
        return false;
    }

    if (index.column() != 0) {
        return false;
    }

    ProjectTreeItem *item = projectItem(index);
    if (!item || item == _rootItem || item == _visibleRootItem) {
        return false;
    }

    // make sure that we're not allowing linefeeds in the name
    QString newName = value.toString().simplified();
    bool ok = _control->renameProject(item->getProject().getID(), newName);
    if (ok && item->setData(index.column(), newName)) {
        emit dataChanged(index, index);
    }
    return ok;

}

Qt::ItemFlags ProjectTreeModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return QAbstractItemModel::flags(index);
    }

    ProjectTreeItem *item = projectItem(index);

    Qt::ItemFlags base = _enableCheckboxes ? Qt::ItemIsUserCheckable : Qt::NoItemFlags;

    // the visible root is not draggable, but accepts drops
    if (item == _visibleRootItem) {
        return base | Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);
    }

    if (item && item->getProject().isValid()) {
        // drag is only allowed to start with the name column
        if (index.column() == ProjectTreeItem::COL_NAME) {
            base |= Qt::ItemIsDragEnabled;
        }

        return base
               | Qt::ItemIsDropEnabled
               | Qt::ItemIsSelectable
               | Qt::ItemIsEnabled
               | Qt::ItemIsEditable
               | QAbstractItemModel::flags(index);
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

    return QAbstractItemModel::headerData(section, orientation, role);
}

QModelIndex ProjectTreeModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent)) {
        return {};
    }

    ProjectTreeItem *parentItem = projectItem(parent);
    if (!parentItem) {
        return {};
    }

    ProjectTreeItem *childItem = parentItem->childAt(row);
    if (!childItem) {
        return {};
    }

    return createIndex(row, column, childItem);
}

QModelIndex ProjectTreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid()) {
        return {};
    }

    auto *item = projectItem(index);
    if (!item || item == _rootItem) {
        return {};
    }

    auto *parentItem = item->parentItem();
    if (!parentItem || parentItem == _rootItem) {
        return {};
    }

    return createIndex(parentItem->row(), 0, parentItem);
}

int ProjectTreeModel::rowCount(const QModelIndex &parent) const {
    if (!parent.isValid()) {
        return _rootItem->childCount();
    }

    if (parent.column() > ProjectTreeItem::COL_NAME) {
        return 0;
    }

    ProjectTreeItem *parentItem = projectItem(parent);
    return parentItem->childCount();
}

ProjectTreeItem *ProjectTreeModel::projectItem(const QModelIndex &index) const {
    if (!index.isValid()) {
        return _rootItem;
    }

    auto *item = static_cast<ProjectTreeItem *>(index.internalPointer());
    return item;
}

void ProjectTreeModel::printProjects(int level, ProjectTreeItem *root) {
    qDebug() << QString(" ").repeated(level) << root->data(0).toString();
    for (int i = 0; i < root->childCount(); ++i) {
        printProjects(level + 1, root->childAt(i));
    }
}

QModelIndex ProjectTreeModel::getProjectRow(const QString &projectID) const {
    const QModelIndexList &list = match(index(0, 0, QModelIndex()), IDRole, projectID, 1,
                                        Qt::MatchExactly | Qt::MatchRecursive);
    if (list.size() == 1) {
        return list.first();
    }
    return {};
}

void ProjectTreeModel::updateProject(const Project &project) {
    const QModelIndex &row = getProjectRow(project.getID());
    if (row.isValid()) {
        ProjectTreeItem *item = projectItem(row);
        if (item) {
            item->refreshWith(project);
            emit dataChanged(row, row.siblingAtColumn(ProjectTreeItem::LAST_COL_INDEX));
        }
    }
}

void ProjectTreeModel::updateProjectStatus(const QString &projectID) {
    const QModelIndex &row = getProjectRow(projectID);
    if (row.isValid()) {
        ProjectTreeItem *item = projectItem(row);
        if (item) {
            emit dataChanged(row.siblingAtColumn(ProjectTreeItem::FIRST_STATUS_COL_INDEX),
                             row.siblingAtColumn(ProjectTreeItem::LAST_COL_INDEX));
        }
    }
}

Project ProjectTreeModel::projectAtIndex(const QModelIndex &index) {
    return projectItem(index)->getProject();
}

void ProjectTreeModel::addProject(const Project &project) {
    if (!project.isValid()) {
        return;
    }

    const QModelIndex &row = getProjectRow(project.getID());
    if (row.isValid()) {
        // project already exists
        return;
    }

    const QList<Project> projects = QList<Project>() << project;

    QModelIndex parentRow = index(0, 0, QModelIndex());
    if (!project.getParentID().isEmpty()) {
        parentRow = getProjectRow(project.getParentID());
    }

    ProjectTreeItem *parentItem = projectItem(parentRow);
    int nextIndex = parentItem->childCount();
    beginInsertRows(parentRow, nextIndex, nextIndex);
    parentItem->appendChild(new ProjectTreeItem(project, _statusManager, parentItem));
    endInsertRows();
}

void ProjectTreeModel::removeProject(const Project &project) {
    if (!project.isValid()) {
        return;
    }

    const QModelIndex &row = getProjectRow(project.getID());
    if (!row.isValid()) {
        return;
    }

    removeRow(row.row(), row.parent());
}

/**
 * The parents of the projects may have changed. Apply the updates where we detect a difference.
 * @param projects
 */
void ProjectTreeModel::onProjectHierarchyChange(const QList<Project> &projects) {
    qDebug() << "onProjectHierarchyChange";
    for (const auto &updated: projects) {
        const QModelIndex &row = getProjectRow(updated.getID());
        const Project &stored = projectAtIndex(row);

        qDebug() << "new parent" << updated.getParentID();
        qDebug() << "old parent" << stored.getParentID();
        if (updated.getParentID() != stored.getParentID()) {
            // move the row
            removeProject(stored);
            addProject(updated);
        }
    }
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
        ProjectTreeItem *treeItem = projectItem(i);
        if (treeItem == nullptr || !treeItem->getProject().isValid()) {
            return nullptr;
        }
        projectIDs << treeItem->getProject().getID();
    }

    auto *result = new QMimeData();
    result->setData(PROJECTS_MIME_TYPE, projectIDs.join("||").toUtf8());
    return result;
}

bool ProjectTreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                    const QModelIndex &parent) {
    if (data->hasFormat(PROJECTS_MIME_TYPE)) {
        return handleDropProjectIDs(data, action, row, column, parent);
    }
    if (data->hasFormat(FRAMES_MIME_TYPE)) {
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

    ProjectTreeItem *parentItem = projectItem(parent);
    if (!parent.isValid() || !parentItem) {
        return false;
    }

    const QString &parentID = parentItem->getProject().getID();

    // don't move data in the model if the data couldn't be changed in tom
    // passing signalHierarchyChange = false because we're handling the change on our own here
    bool success = _control->updateProjects(ids, false, "", true, parentID, false, "", false);
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
        parentItem->insertChild(projectItem(rowIndex), 0);
        endInsertRows();
    }

    return true;
}

bool ProjectTreeModel::handleDropFrameIDs(const QMimeData *data, Qt::DropAction action, int /*row*/, int /*column*/,
                                          const QModelIndex &parent) {
    if (!parent.isValid() || action != Qt::MoveAction) {
        return false;
    }

    ProjectTreeItem *parentItem = projectItem(parent);
    if (!parentItem || parentItem == _visibleRootItem || !parentItem->getProject().isValid()) {
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

    // first item is a comma-seperated list of strings
    const QStringList &sourceProjectIDs = ids.takeFirst().split(',');
    const QString &parentProjectID = parentItem->getProject().getID();

    qDebug() << "moving frames, projects" << sourceProjectIDs << "ids" << ids;

    // don't move data in the model if the data couldn't be changed in tom
    bool success = _control->updateFrames(ids, sourceProjectIDs,
                                          false, QDateTime(),
                                          false, QDateTime(),
                                          false, "",
                                          true, parentProjectID,
                                          false, false);
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
    ProjectTreeItem *parentItem = projectItem(parent);
    qDebug() << "inserting rows at" << row << "count" << count << "into" << parentItem->getProject().getName();

    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; i++) {
        parentItem->insertChild(new ProjectTreeItem(Project(), _statusManager, parentItem), i);
    }
    endInsertRows();

    return true;
}

bool ProjectTreeModel::removeRows(int row, int count, const QModelIndex &parent) {
    qDebug() << "removing rows" << row << "to" << row + count - 1 << "from"
             << projectItem(parent)->getProject().getName();
    if (!parent.isValid()) {
        return false;
    }

    ProjectTreeItem *parentItem = projectItem(parent);
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

void ProjectTreeModel::setShowArchived(bool showArchived) {
    _showArchived = showArchived;
}

