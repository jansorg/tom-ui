#include <model/frametableviewmodel.h>
#include <model/project_tree_item.h>
#include <QtWidgets/QMenu>
#include <icons.h>
#include "frametableview.h"

FrameTableView::FrameTableView(QWidget *parent) : QTableView(parent) {
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void FrameTableView::setup(GotimeControl *control) {
    _control = control;

    _sourceModel = new FrameTableViewModel(_control, this);

    _sortedModel = new QSortFilterProxyModel(this);
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
    QAction *stop = menu.addAction(Icons::stop(), "Stop", [this, frame] { _control->updateFrame(frame, false, QDateTime(), true, QDateTime::currentDateTime(), false, ""); });
    stop->setEnabled(frame->isRunning());

    menu.addSeparator();
    menu.addAction(Icons::removeFrame(), "Delete", [this, frame] { qDebug() << "removing frame" << frame->notes; _control->removeFrame(*frame); });

    menu.exec(globalPos);
}

void FrameTableView::onProjectSelected(const Project &project) {
    _sourceModel->loadFrames(project);
}
