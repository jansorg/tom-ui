#include <QtWidgets/QMenu>
#include <QtWidgets/QHeaderView>
#include <QtTest/QAbstractItemModelTester>
#include <QtWidgets/QLabel>
#include <source/model/UserRoles.h>
#include <QtWidgets/QStyledItemDelegate>
#include <source/frameEditor/FrameEditorDialog.h>

#include "model/FrameTableViewModel.h"
#include "model/ProjectTreeItem.h"
#include "model/FrameTableSortFilterModel.h"
#include "FrameTableView.h"
#include "icons.h"
#include "IconItemDelegate.h"

FrameTableView::FrameTableView(QWidget *parent) : QTableView(parent), _control(nullptr), _proxyModel(nullptr), _sourceModel(nullptr), _statusManager(nullptr) {
    setContextMenuPolicy(Qt::CustomContextMenu);

    setDragDropMode(QTableView::DragOnly);
    setDragEnabled(true);
    setAcceptDrops(false);

    _deleteSelectedAction = new QAction(Icons::timeEntryDelete(), "Delete selected", this);
    _deleteSelectedAction->setShortcutContext(Qt::WindowShortcut);
    _deleteSelectedAction->setShortcuts(QKeySequence::Delete);
    _deleteSelectedAction->setIcon(Icons::timeEntryDelete());
    connect(_deleteSelectedAction, &QAction::triggered, this, &FrameTableView::deleteSelectedEntries);

    setItemDelegateForColumn(FrameTableViewModel::COL_ARCHIVED, new IconItemDelegate(Icons::timeEntryArchive(), this));
}

void FrameTableView::setup(TomControl *control, ProjectStatusManager *statusManager) {
    _control = control;
    _statusManager = statusManager;

    _sourceModel = new FrameTableViewModel(_control, this);
    _proxyModel = new FrameTableSortFilterModel(this);
    _proxyModel->setSourceModel(_sourceModel);
    setModel(_proxyModel);

    // new QAbstractItemModelTester(_sourceModel, QAbstractItemModelTester::FailureReportingMode::Warning, this);

    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setMinimumSectionSize(0);
    verticalHeader()->setDefaultSectionSize(verticalHeader()->defaultSectionSize() - 4);

    horizontalHeader()->setResizeContentsPrecision(1);
    horizontalHeader()->setSectionResizeMode(FrameTableViewModel::COL_ARCHIVED, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(FrameTableViewModel::COL_START_DATE, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(FrameTableViewModel::COL_START, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(FrameTableViewModel::COL_END, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(FrameTableViewModel::COL_DURATION, QHeaderView::ResizeToContents);
    setColumnWidth(FrameTableViewModel::COL_SUBPROJECT, 17 * fontMetrics().averageCharWidth());

    hideColumn(FrameTableViewModel::COL_TAGS);
    sortByColumn(FrameTableViewModel::COL_START, Qt::DescendingOrder);

    connect(this, &FrameTableView::customContextMenuRequested, this, &FrameTableView::onCustomContextMenuRequested);
    connect(_sourceModel, &FrameTableViewModel::subprojectStatusChange, this, &FrameTableView::onSubprojectStatusChange);
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
    auto *stop = menu.addAction(Icons::stopTimer(), "Stop", [this] {
        _control->stopActivity();
    });
    stop->setEnabled(frame->isActive());
    menu.addSeparator();
    menu.addAction(Icons::frameEdit(), "Edit frame...", [this, frame] { FrameEditorDialog::show(*frame, _control, _statusManager, this); });
    menu.addAction(_deleteSelectedAction);
    menu.addSeparator();
    menu.addAction(Icons::timeEntryArchive(), "Archive", this, &FrameTableView::archiveSelectedEntries);
    menu.exec(globalPos);
}

void FrameTableView::onProjectSelected(const Project &project) {
    _sourceModel->loadFrames(project);
}

void FrameTableView::onSubprojectStatusChange(bool available) {
    if (available) {
        showColumn(FrameTableViewModel::COL_SUBPROJECT);
    } else {
        hideColumn(FrameTableViewModel::COL_SUBPROJECT);
    }
}

void FrameTableView::deleteSelectedEntries() {
    auto frames = selectedFrames();
    if (!frames.isEmpty()) {
        _control->removeFrames(frames);
    }
}

void FrameTableView::archiveSelectedEntries() {
    auto frames = selectedFrames();
    if (!frames.isEmpty()) {
        _control->updateFrame(frames, false, QDateTime(), false, QDateTime(), false, "", false, "", true, true);
    }
}

int FrameTableView::sizeHintForColumn(int column) const {
    if (column == FrameTableViewModel::COL_ARCHIVED) {
        // smaller padding
        return 16 + 10;
    }

    const QFontMetrics &metrics = fontMetrics();
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

    int padding = 16;
    return result + padding;
}

void FrameTableView::setShowArchived(bool showArchived) {
    if (showArchived) {
        showColumn(FrameTableViewModel::COL_ARCHIVED);
    } else {
        hideColumn(FrameTableViewModel::COL_ARCHIVED);
    }
    _sourceModel->setShowArchived(showArchived);
}

QList<Frame *> FrameTableView::selectedFrames() const {
    const QModelIndexList &rows = selectionModel()->selectedRows(FrameTableViewModel::FIRST_COL);
    if (rows.isEmpty()) {
        return QList<Frame *>();
    }

    QList<Frame *> frames;
    for (auto row: rows) {
        auto sourceRow = _proxyModel->mapToSource(row);
        if (Frame *frame = _sourceModel->frameAt(sourceRow)) {
            frames << frame;
        }
    }
    return frames;
}

QAction *FrameTableView::getDeleteAction() {
    return _deleteSelectedAction;
}

void FrameTableView::readSettings() {
}

void FrameTableView::writeSettings() {
}
