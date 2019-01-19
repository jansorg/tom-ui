#include <QtWidgets/QMenu>
#include <QtWidgets/QHeaderView>
#include <icons.h>
#include <QtWidgets/QInputDialog>

#include "projecttreeview.h"

ProjectTreeView::ProjectTreeView(QWidget *parent) : QTreeView(parent) {
    setContextMenuPolicy(Qt::CustomContextMenu);
    setUniformRowHeights(true);

    connect(this, &ProjectTreeView::customContextMenuRequested, this, &ProjectTreeView::onCustomContextMenuRequested);
}

void ProjectTreeView::setup(GotimeControl *control, ProjectStatusManager *statusManager) {
    _control = control;
    _statusManager = statusManager;

    _sourceModel = new ProjectTreeModel(_control, _statusManager, this);

    _sortModel = new QSortFilterProxyModel(this);
    _sortModel->setSourceModel(_sourceModel);
    setModel(_sortModel);

    header()->setStretchLastSection(false);
    header()->setSectionResizeMode(0, QHeaderView::Stretch);
    header()->setCascadingSectionResizes(true);
    sortByColumn(0, Qt::AscendingOrder);

    connect(selectionModel(), &QItemSelectionModel::currentRowChanged, this, &ProjectTreeView::onCurrentChanged);
    connect(_control, &GotimeControl::projectUpdated, this, &ProjectTreeView::projectUpdated);
    connect(_control, &GotimeControl::projectCreated, this, &ProjectTreeView::refresh);
    connect(_statusManager, &ProjectStatusManager::projectsStatusChanged, this, &ProjectTreeView::projectsStatusChanged);
}

void ProjectTreeView::onCurrentChanged(const QModelIndex &index, const QModelIndex &) {
    auto *item = static_cast<ProjectTreeItem *>(index.internalPointer());
    if (item && item->getProject().isValid()) {
        emit projectSelected(item->getProject());
    } else {
        emit projectSelected(Project());
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
    QAction *start = menu.addAction(Icons::start(), "Start", [this, project] { _control->startProject(project); });
    QAction *stop = menu.addAction(Icons::stop(), "Stop", [this] { _control->stopActivity(); });
    menu.addSeparator();
    menu.addAction(Icons::newProject(), "Create new project", [this, project] { createNewProject(project); } );

    bool started = _control->isStarted(project);
    start->setEnabled(!started);
    stop->setEnabled(started);

    menu.exec(globalPos);
}

void ProjectTreeView::refresh() {
    _sourceModel->loadProjects();
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
    bool ok;
    QString projectName = QInputDialog::getText(this, tr("Create Project"), tr("Project name:"), QLineEdit::Normal, "", &ok);
    if (ok && !projectName.isEmpty()){
        _control->createProject(parentProject.getID(), projectName);
    }
}
