#include <QtWidgets/QMenu>
#include <QtWidgets/QHeaderView>
#include <QtTest/QAbstractItemModelTester>

#include "model/FrameTableViewModel.h"
#include "model/ProjectTreeItem.h"
#include "model/FrameTableSortFilterModel.h"
#include "FrameTableView.h"
#include "icons.h"

FrameTableView::FrameTableView(QWidget *parent) : QTableView(parent) {
    setContextMenuPolicy(Qt::CustomContextMenu);

    setDragDropMode(QTableView::DragOnly);
    setDragEnabled(true);
    setAcceptDrops(false);
}

void FrameTableView::setup(TomControl *control) {
    _control = control;

    _sourceModel = new FrameTableViewModel(_control, this);
    _proxyModel = new FrameTableSortFilterModel(this);
    _proxyModel->setSourceModel(_sourceModel);
    setModel(_proxyModel);

//    new QAbstractItemModelTester(_sourceModel, QAbstractItemModelTester::FailureReportingMode::Warning, this);

    horizontalHeader()->setResizeContentsPrecision(1);
    horizontalHeader()->setSectionResizeMode(FrameTableViewModel::COL_START_DATE, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(FrameTableViewModel::COL_START, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(FrameTableViewModel::COL_END, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(FrameTableViewModel::COL_DURATION, QHeaderView::ResizeToContents);

    // horizontalHeader()->setSectionResizeMode(FrameTableViewModel::COL_SUBPROJECT, QHeaderView::ResizeToContents);
    setColumnWidth(FrameTableViewModel::COL_SUBPROJECT, 17 * fontMetrics().averageCharWidth());

    sortByColumn(0, Qt::DescendingOrder);

    connect(this, &FrameTableView::customContextMenuRequested, this, &FrameTableView::onCustomContextMenuRequested);
}

void FrameTableView::onCustomContextMenuRequested(const QPoint &pos) {
    const QModelIndex &index = indexAt(pos);

    if (index.isValid()) {
        const QModelIndex &sourceIndex = _proxyModel->mapToSource(index);
        Frame *frame = _sourceModel->frameAt(sourceIndex);

        showContextMenu(frame, viewport()->mapToGlobal(pos));
    }
}

void FrameTableView::showContextMenu(Frame *frame, QPoint globalPos) {
    QMenu menu;
    QAction *stop = menu.addAction(Icons::stopTimer(), "Stop", [this, frame] {
        _control->updateFrame(QList<Frame *>() << frame,
                              false, QDateTime(),
                              true, QDateTime::currentDateTime(),
                              false, "",
                              false, "");
    });
    stop->setEnabled(frame->isRunning());

    menu.addSeparator();
    menu.addAction(Icons::removeFrame(), "Delete", this, &FrameTableView::deleteSelectedEntries)->setIconVisibleInMenu(true);

    menu.exec(globalPos);
}

void FrameTableView::onProjectSelected(const Project &project) {
    _sourceModel->loadFrames(project);
}

void FrameTableView::deleteSelectedEntries() {
    const QModelIndexList &rows = selectionModel()->selectedRows(FrameTableViewModel::FIRST_COL);
    if (rows.isEmpty()) {
        return;
    }

    QList<Frame *> frames;
    for (auto row: rows) {
        auto sourceRow = _proxyModel->mapToSource(row);
        Frame *frame = _sourceModel->frameAt(sourceRow);
        if (frame) {
            frames << frame;
        }
    }

    if (!frames.isEmpty()) {
        _control->removeFrames(frames);
    }
}

int FrameTableView::sizeHintForColumn(int column) const {
    const QFontMetrics &metrics = fontMetrics();

    int padding = 25;
    int result = 0;

    if (column == FrameTableViewModel::COL_START_DATE) {
        QString sample = QDateTime::currentDateTime().date().toString(Qt::SystemLocaleShortDate);
        result = metrics.width(sample);
    } else if (column == FrameTableViewModel::COL_START) {
        QString sample = QDateTime::currentDateTime().time().toString(Qt::SystemLocaleShortDate);
        result = metrics.width(sample);
    } else if (column == FrameTableViewModel::COL_END) {
        QString sample = QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate);
        result = metrics.width(sample);
    } else if (column == FrameTableViewModel::COL_DURATION) {
        const QString &sample = Timespan(1000 * 60 * 60 * 10).format();
        result = metrics.width(sample);
    } else if (column == FrameTableViewModel::COL_SUBPROJECT) {
        result = metrics.averageCharWidth() * 20;
    } else {
        result = QTableView::sizeHintForColumn(column);
    }

    return result + padding;
}
