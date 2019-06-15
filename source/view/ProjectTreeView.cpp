#include <QtWidgets/QMenu>
#include <QtWidgets/QHeaderView>
#include <icons.h>
#include <QtWidgets/QInputDialog>
#include <dialogs/CommonDialogs.h>
#include <model/ProjectTreeSortFilterModel.h>
#include <ActionUtils.h>
#include <QtTest/QAbstractItemModelTester>
#include <source/projectEditor/ProjectEditorDialog.h>
#include <source/model/UserRoles.h>

#include "ProjectTreeView.h"

ProjectTreeView::ProjectTreeView(QWidget *parent) : QTreeView(parent) {
    setContextMenuPolicy(Qt::CustomContextMenu);
    setUniformRowHeights(true);

    setDragDropMode(QAbstractItemView::DragDrop);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setRootIsDecorated(false);

    connect(this, &ProjectTreeView::customContextMenuRequested, this, &ProjectTreeView::onCustomContextMenuRequested);

    _deleteSelectedAction = new QAction(Icons::projectRemove(), tr("Delete project..."), this);
    _deleteSelectedAction->setShortcutContext(Qt::WindowShortcut);
    _deleteSelectedAction->setShortcuts(QKeySequence::Delete);
    connect(_deleteSelectedAction, &QAction::triggered, this, &ProjectTreeView::deleteSelectedProject);
}

void ProjectTreeView::setup(TomControl *control, ProjectStatusManager *statusManager) {
    _control = control;
    _statusManager = statusManager;

    setSortingEnabled(true);

    _sourceModel = new ProjectTreeModel(_control, _statusManager, true, this);

    _proxyModel = new ProjectTreeSortFilterModel(this);
    _proxyModel->setSourceModel(_sourceModel);

    setModel(_proxyModel);

    _sourceModel->loadProjects();

//    new QAbstractItemModelTester(_proxyModel, QAbstractItemModelTester::FailureReportingMode::Fatal, this);

    header()->setStretchLastSection(false);
    header()->setSectionResizeMode(0, QHeaderView::Stretch);
    header()->setCascadingSectionResizes(true);

    connect(selectionModel(), &QItemSelectionModel::currentRowChanged, this, &ProjectTreeView::onCurrentChanged);
    connect(_control, &TomControl::projectUpdated, this, &ProjectTreeView::projectUpdated);
    connect(_statusManager, &ProjectStatusManager::projectsStatusChanged, this, &ProjectTreeView::projectsStatusChanged);
}

void ProjectTreeView::onCurrentChanged(const QModelIndex &index, const QModelIndex &) {
    auto sourceIndex = _proxyModel->mapToSource(index);
    if (!sourceIndex.isValid()) {
        emit projectSelected(Project());
    } else {
        emit projectSelected(_sourceModel->projectAtIndex(sourceIndex));
    }
}

void ProjectTreeView::onCustomContextMenuRequested(const QPoint &pos) {
    const QModelIndex &index = indexAt(pos);

    if (index.isValid()) {
        ProjectTreeItem *item = _sourceModel->projectItem(_proxyModel->mapToSource(index));

        // Note: We must map the point to global from the viewport to account for the header.
        showContextMenu(item, viewport()->mapToGlobal(pos));
    }
}

void ProjectTreeView::showContextMenu(ProjectTreeItem *item, const QPoint &globalPos) {
    const Project &project = item->getProject();

    QMenu menu;
    QAction *start = menu.addAction(Icons::startTimer(), "Start timer", [this, project] { _control->startProject(project); });
    QAction *stop = menu.addAction(Icons::stopTimer(), "Stop timer", [this] { _control->stopActivity(); });
    menu.addSeparator();
    menu.addAction(Icons::projectNew(), "Create new subproject...", [this, project] { createNewProject(project); });
    menu.addAction(Icons::projectEdit(), "Edit project...", [this, project] { ProjectEditorDialog::show(project, _control, _statusManager, this); });
    menu.addAction(_deleteSelectedAction);
    menu.addSeparator();
    menu.addAction(Icons::timeEntryArchive(), "Archive all entries", [this, project] { _control->archiveProjectFrames(project, true); });

    bool started = project.isValid() && _control->isStarted(project);
    start->setEnabled(project.isValid() && !started);
    stop->setEnabled(started);

    menu.exec(globalPos);
}

void ProjectTreeView::refresh() {
    reset();
    _proxyModel->invalidate();
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

    const QModelIndex &sourceIndex = _proxyModel->mapToSource(current);
    ProjectTreeItem *item = _sourceModel->projectItem(sourceIndex);
    if (!item) {
        return Project();
    }

    return item->getProject();
}

void ProjectTreeView::selectProject(const Project &project) {
    if (project.isValid()) {
        const QModelIndex &source = _proxyModel->mapFromSource(_sourceModel->getProjectRow(project.getID()));
        setCurrentIndex(source);
        scrollTo(source, PositionAtCenter);
    } else {
        clearSelection();
    }
}

void ProjectTreeView::setShowArchived(bool showArchived) {
    _sourceModel->setShowArchived(showArchived);
}

void ProjectTreeView::readSettings() {
    QSettings settings;
    const QString &id = settings.value("projectTree/selected").toString();
    if (!id.isEmpty()) {
        const QModelIndex &row = _sourceModel->getProjectRow(id);
        if (row.isValid()) {
            auto proxyRow = _proxyModel->mapFromSource(row);
            setCurrentIndex(proxyRow);
            scrollTo(proxyRow, PositionAtCenter);
        }
    }
}

void ProjectTreeView::writeSettings() {
    QStringList ids;
    for (auto row : selectionModel()->selectedRows()) {
        if (row.isValid()) {
            ids << row.data(UserRoles::IDRole).toString();
        }
    }

    QSettings settings;
    settings.setValue("projectTree/selected", ids);
}

void ProjectTreeView::setShowYesterdayColumn(bool show) {
    setColumnHidden(ProjectTreeItem::COL_YESTERDAY, !show);
}

void ProjectTreeView::setShowTodayColumn(bool show) {
    setColumnHidden(ProjectTreeItem::COL_TODAY, !show);
}

void ProjectTreeView::setShowWeekColumn(bool show) {
    setColumnHidden(ProjectTreeItem::COL_WEEK, !show);
}

void ProjectTreeView::setShowMonthColumn(bool show) {
    setColumnHidden(ProjectTreeItem::COL_MONTH, !show);
}

void ProjectTreeView::setShowYearColumn(bool show) {
    setColumnHidden(ProjectTreeItem::COL_YEAR, !show);
}

void ProjectTreeView::setShowTotalColumn(bool show) {
    setColumnHidden(ProjectTreeItem::COL_TOTAL, !show);
}

void ProjectTreeView::deleteSelectedProject() {
    const QModelIndexList &rows = selectionModel()->selectedRows(ProjectTreeItem::COL_NAME);
    if (rows.isEmpty()) {
        return;
    }

    const QModelIndex &sourceSelection = _proxyModel->mapToSource(rows.first());
    const Project &project = _sourceModel->projectAtIndex(sourceSelection);
    if (project.isValid()) {
        ActionUtils::removeProject(_control, project, this);
    }
}

void ProjectTreeView::dragMoveEvent(QDragMoveEvent *event) {
    QTreeView::dragMoveEvent(event);
    if (!event->isAccepted()) {
        return;
    }

    event->setAccepted(dropIndicatorPosition() == OnItem);
}

QAction *ProjectTreeView::getDeleteAction() const {
    return _deleteSelectedAction;
}
