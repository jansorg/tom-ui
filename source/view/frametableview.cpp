#include <model/frametableviewmodel.h>
#include "frametableview.h"

FrameTableView::FrameTableView(QWidget *parent) : QTableView(parent) {
}

void FrameTableView::setup(GotimeControl *control) {
    _control = control;

    connect(_control, &GotimeControl::projectUpdated, this, &FrameTableView::projectUpdated);
}

void FrameTableView::projectUpdated(const Project &project) {
    if (project == _currentProject) {
        loadFrames(project);
    }
}

void FrameTableView::loadFrames(const Project &project) {
    _currentProject = project;

    if (!project.isValid()) {
        setModel(nullptr);
        return;
    }

    auto frames = _control->loadFrames(project.getID(), true);

    auto *sortedModel = new QSortFilterProxyModel(this);
    auto *frameModel = new FrameTableViewModel(frames, _control, this);
    sortedModel->setSourceModel(frameModel);
    setModel(sortedModel);
    sortByColumn(0, Qt::DescendingOrder);
}
