#include <QtWidgets/QMenu>

#include "gotime/startStop_project_action.h"
#include "model/ProjectTreeModel.h"
#include "projecttreeview.h"

ProjectTreeView::ProjectTreeView(QWidget *parent) : QTreeView(parent) {
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void ProjectTreeView::setControl(GotimeControl *control) {
    connect(this,
            &QTreeView::customContextMenuRequested,
            this,
            &ProjectTreeView::onCustomContextMenuRequested);
    _control = control;
}

void ProjectTreeView::onCustomContextMenuRequested(const QPoint &pos) {
    const QModelIndex &index = indexAt(pos);

    if (index.isValid()) {
        auto *model = dynamic_cast<ProjectTreeModel *>(this->model());
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
