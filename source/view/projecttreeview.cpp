#include <QtWidgets/QMenu>
#include <QtWidgets/QHeaderView>

#include "projecttreeview.h"

#include "gotime/projectstatusmanager.h"
#include "model/ProjectTreeModel.h"

ProjectTreeView::ProjectTreeView(QWidget *parent) : QTreeView(parent) {
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &ProjectTreeView::customContextMenuRequested, this, &ProjectTreeView::onCustomContextMenuRequested);
}

void ProjectTreeView::setup(GotimeControl *control, ProjectStatusManager *statusManager) {
    _control = control;
    _statusManager = statusManager;

    connect(_control, &GotimeControl::projectUpdated, this, &ProjectTreeView::projectUpdated);
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
        auto *model = getProjectModel();
        ProjectTreeItem *item = model->getItem(index);

        // Note: We must map the point to global from the viewport to account for the header.
        showContextMenu(item, viewport()->mapToGlobal(pos));
    }
}

void ProjectTreeView::showContextMenu(ProjectTreeItem *item, const QPoint &globalPos) {
    const Project &project = item->getProject();

    QMenu menu;
    QAction *start = menu.addAction(QIcon(":/images/start.svg"), "Start", [this, project] { _control->startProject(project); });
    QAction *stop = menu.addAction(QIcon(":/images/stop.svg"), "Stop", [this] { _control->stopActivity(); });

    bool started = _control->isStarted(project);
    start->setEnabled(!started);
    stop->setEnabled(started);

    menu.exec(globalPos);
}

void ProjectTreeView::refresh() {
    //fixme delete old model?
    auto *model = new ProjectTreeModel(_control, _statusManager, this);

    this->setModel(model);
    this->sortByColumn(0, Qt::AscendingOrder);
    this->header()->setStretchLastSection(false);
    this->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    this->header()->setCascadingSectionResizes(true);

    connect(this->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &ProjectTreeView::onCurrentChanged);
}

void ProjectTreeView::projectUpdated(const Project &project) {
    auto *model = getProjectModel();
    model->updateProject(project);
}

void ProjectTreeView::projectsStatusChanged(const QStringList &projectIDs) {
    auto *model = getProjectModel();
    for (const auto &id : projectIDs) {
        model->updateProjectStatus(id);
    }
}

ProjectTreeModel *ProjectTreeView::getProjectModel() {
    return dynamic_cast<ProjectTreeModel *>(this->model());
}
