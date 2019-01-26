#include <QtWidgets/QMenu>
#include <QtWidgets/QHeaderView>
#include <icons.h>
#include <QtWidgets/QInputDialog>
#include <dialogs/CommonDialogs.h>
#include <model/ProjectTreeSortFilterModel.h>

#include "ProjectTreeView.h"

ProjectTreeView::ProjectTreeView(QWidget *parent) : QTreeView(parent) {
    setContextMenuPolicy(Qt::CustomContextMenu);
    setUniformRowHeights(true);
    setSortingEnabled(true);

    setDragDropMode(QAbstractItemView::DragDrop);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);

    connect(this, &ProjectTreeView::customContextMenuRequested, this, &ProjectTreeView::onCustomContextMenuRequested);
}

void ProjectTreeView::setup(TomControl *control, ProjectStatusManager *statusManager) {
    _control = control;
    _statusManager = statusManager;

    _sourceModel = new ProjectTreeModel(_control, _statusManager, this);

    _sortModel = new ProjectTreeSortFilterModel(this);
    _sortModel->setSourceModel(_sourceModel);
    setModel(_sortModel);

    header()->setStretchLastSection(false);
    header()->setSectionResizeMode(0, QHeaderView::Stretch);
    header()->setCascadingSectionResizes(true);
    sortByColumn(0, Qt::AscendingOrder);

    connect(selectionModel(), &QItemSelectionModel::currentRowChanged, this, &ProjectTreeView::onCurrentChanged);
    connect(_control, &TomControl::projectUpdated, this, &ProjectTreeView::projectUpdated);
    connect(_statusManager, &ProjectStatusManager::projectsStatusChanged, this, &ProjectTreeView::projectsStatusChanged);
}

void ProjectTreeView::onCurrentChanged(const QModelIndex &index, const QModelIndex &) {
    auto sourceIndex = _sortModel->mapToSource(index);
    if (!sourceIndex.isValid()) {
        emit projectSelected(Project());
    } else {
        emit projectSelected(_sourceModel->projectAtIndex(sourceIndex));
    }

}

void ProjectTreeView::onCustomContextMenuRequested(const QPoint &pos) {
    const QModelIndex &index = indexAt(pos);

    if (index.isValid()) {
        ProjectTreeItem *item = _sourceModel->getItem(_sortModel->mapToSource(index));

        // Note: We must map the point to global from the viewport to account for the header.
        showContextMenu(item, viewport()->mapToGlobal(pos));
    }
}

void ProjectTreeView::showContextMenu(ProjectTreeItem *item, const QPoint &globalPos) {
    const Project &project = item->getProject();

    QMenu menu;
    QAction *start = menu.addAction(Icons::startTimer(), "Start",
                                    [this, project] { _control->startProject(project); });
    QAction *stop = menu.addAction(Icons::stopTimer(), "Stop", [this] { _control->stopActivity(); });
    stop->setIconVisibleInMenu(true);
    menu.addSeparator();
    menu.addAction(Icons::projectNew(), "Create new subproject", [this, project] { createNewProject(project); })->setIconVisibleInMenu(true);

    bool started = _control->isStarted(project);
    start->setEnabled(!started);
    stop->setEnabled(started);

    menu.exec(globalPos);
}

void ProjectTreeView::refresh() {
    _sourceModel->loadProjects();

    expandToDepth(0);
}

void ProjectTreeView::projectUpdated(const Project &project) {
    _sourceModel->updateProject(project);
}

void ProjectTreeView::projectsStatusChanged(const QStringList &projectIDs) {
    for (const auto &id : projectIDs) {
        _sourceModel->updateProjectStatus(id);
    }
}

void ProjectTreeView::createNewProject(const Project &parentProject) {
    CommonDialogs::createProject(parentProject, _control, this);
}

const Project ProjectTreeView::getCurrentProject() {
    const QModelIndex &current = currentIndex();
    if (!current.isValid()) {
        return Project();
    }
    
    const QModelIndex &sourceIndex = _sortModel->mapToSource(current);
    ProjectTreeItem *item = _sourceModel->getItem(sourceIndex);
    if (!item) {
        return Project();
    }

    return item->getProject();
}
