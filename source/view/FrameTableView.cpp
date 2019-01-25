#include <model/FrameTableViewModel.h>
#include <model/ProjectTreeItem.h>
#include <QtWidgets/QMenu>
#include <icons.h>
#include <model/FrameTableSortFilterModel.h>
#include "FrameTableView.h"

FrameTableView::FrameTableView(QWidget *parent) : QTableView(parent) {
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void FrameTableView::setup(TomControl *control) {
    _control = control;

    _sourceModel = new FrameTableViewModel(_control, this);

    _sortedModel = new FrameTableSortFilterModel(this);
    _sortedModel->setSourceModel(_sourceModel);
    setModel(_sortedModel);

    sortByColumn(0, Qt::DescendingOrder);

    connect(this, &FrameTableView::customContextMenuRequested, this, &FrameTableView::onCustomContextMenuRequested);
}

void FrameTableView::onCustomContextMenuRequested(const QPoint &pos) {
    const QModelIndex &index = indexAt(pos);

    if (index.isValid()) {
        const QModelIndex &sourceIndex = _sortedModel->mapToSource(index);
        Frame *frame = _sourceModel->frameAt(sourceIndex);

        showContextMenu(frame, viewport()->mapToGlobal(pos));
    }
}

void FrameTableView::showContextMenu(Frame *frame, QPoint globalPos) {
    QMenu menu;
    QAction *stop = menu.addAction(Icons::stopTimer(), "Stop", [this, frame] { _control->updateFrame(QList<Frame*>() << frame, false, QDateTime(), true, QDateTime::currentDateTime(), false, "", false, ""); });
    stop->setEnabled(frame->isRunning());

    menu.addSeparator();
    menu.addAction(Icons::removeFrame(), "Delete", [this, frame] { qDebug() << "removing frame" << frame->id; _control->removeFrame(*frame); });

    menu.exec(globalPos);
}

void FrameTableView::onProjectSelected(const Project &project) {
    _sourceModel->loadFrames(project);
}
