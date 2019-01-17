#include <QtWidgets/QMenu>
#include <QtWidgets/QHeaderView>

#include "gotime/startStop_project_action.h"
#include "model/ProjectTreeModel.h"
#include "projecttreeview.h"

ProjectTreeView::ProjectTreeView(QWidget *parent) : QTreeView(parent) {
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &ProjectTreeView::activated, this, &ProjectTreeView::onProjectSelected);
    connect(this, &ProjectTreeView::customContextMenuRequested, this, &ProjectTreeView::onCustomContextMenuRequested);
}

void ProjectTreeView::setControl(GotimeControl *control) {
    _control = control;
    connect(_control, &GotimeControl::projectUpdated, this, &ProjectTreeView::projectUpdated);
}

void ProjectTreeView::onProjectSelected(const QModelIndex &index) {
    auto *item = static_cast<ProjectTreeItem *>(index.internalPointer());
    if (item && item->getProject().isValid()) {
        emit projectSelected(item->getProject());
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
    QAction *start = menu.addAction(QIcon(":/images/start.svg"), "Start",
                                    [this, project] { _control->startProject(project); });
    QAction *stop = menu.addAction(QIcon(":/images/stop.svg"), "Stop", [this] { _control->stopActivity(); });


    bool started = _control->isStarted(project);
    start->setEnabled(!started);
    stop->setEnabled(started);

    menu.exec(globalPos);
}

void ProjectTreeView::refresh() {
    //fixme delete old model?
    auto *model = new ProjectTreeModel(_control, this);

    this->setModel(model);
    this->sortByColumn(0, Qt::AscendingOrder);
    this->header()->setStretchLastSection(false);
    this->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    this->header()->setCascadingSectionResizes(true);
}

void ProjectTreeView::projectUpdated(const Project &project) {
    auto *model = getProjectModel();
    model->updateProject(project);
}

ProjectTreeModel *ProjectTreeView::getProjectModel() {
    return dynamic_cast<ProjectTreeModel *>(this->model());
}
