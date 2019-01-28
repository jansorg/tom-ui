#include <QtWidgets/QMenu>
#include <QtWidgets/QHeaderView>
#include <icons.h>
#include <QtWidgets/QInputDialog>
#include <dialogs/CommonDialogs.h>
#include <model/ProjectTreeSortFilterModel.h>
#include <ActionUtils.h>
#include <QtTest/QAbstractItemModelTester>

#include "ProjectTreeView.h"

ProjectTreeView::ProjectTreeView(QWidget *parent) : QTreeView(parent) {
    setContextMenuPolicy(Qt::CustomContextMenu);
    setUniformRowHeights(true);

    setDragDropMode(QAbstractItemView::DragDrop);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);

    connect(this, &ProjectTreeView::customContextMenuRequested, this, &ProjectTreeView::onCustomContextMenuRequested);
}

void ProjectTreeView::setup(TomControl *control, ProjectStatusManager *statusManager) {
    _control = control;
    _statusManager = statusManager;

    setSortingEnabled(true);

    _sourceModel = new ProjectTreeModel(_control, _statusManager, this);

    _proxyModel = new ProjectTreeSortFilterModel(this);
    _proxyModel->setSourceModel(_sourceModel);

    setModel(_proxyModel);

    _sourceModel->loadProjects();

    new QAbstractItemModelTester(_proxyModel, QAbstractItemModelTester::FailureReportingMode::Warning, this);

    connect(this, &QTreeView::expanded, [] { qDebug() << "item expanded"; });
    connect(this, &QTreeView::collapsed, [] { qDebug() << "item collapsed"; });

    connect(_proxyModel, &QSortFilterProxyModel::sourceModelChanged, [] { qDebug() << "source model changes"; });
    connect(_proxyModel, &QSortFilterProxyModel::modelReset, [] { qDebug() << "model reset"; });
    connect(_proxyModel, &QSortFilterProxyModel::modelAboutToBeReset, [] { qDebug() << "model about to reset"; });
    connect(_proxyModel, &QSortFilterProxyModel::rowsMoved, [] { qDebug() << "rows moved"; });
    connect(_proxyModel, &QSortFilterProxyModel::rowsRemoved, [] { qDebug() << "rows removed"; });
    connect(_proxyModel, &QSortFilterProxyModel::rowsInserted, [] { qDebug() << "rows inserted"; });
    connect(_proxyModel, &QSortFilterProxyModel::layoutAboutToBeChanged, []{qDebug() << "layout to be changed";});
    connect(_proxyModel, &QSortFilterProxyModel::layoutChanged, [](const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint) { qDebug() << "layout changed"<<parents<<hint; });
    connect(_proxyModel, &QSortFilterProxyModel::dataChanged, [] { qDebug() << "data changed"; });
    connect(_proxyModel, &QSortFilterProxyModel::headerDataChanged, [] { qDebug() << "header data changed"; });

//    connect(_sortModel, &QSortFilterProxyModel::layoutChanged, [this](const QList<QPersistentModelIndex> &, QAbstractItemModel::LayoutChangeHint hint) { if (hint == QAbstractItemModel::VerticalSortHint){expandToDepth(0);} });

    header()->setStretchLastSection(false);
    header()->setSectionResizeMode(0, QHeaderView::Stretch);
    header()->setCascadingSectionResizes(true);

    connect(selectionModel(), &QItemSelectionModel::currentRowChanged, this, &ProjectTreeView::onCurrentChanged);
    connect(_control, &TomControl::projectUpdated, this, &ProjectTreeView::projectUpdated);
    connect(_statusManager, &ProjectStatusManager::projectsStatusChanged, this, &ProjectTreeView::projectsStatusChanged);
}

void ProjectTreeView::onCurrentChanged(const QModelIndex &index, const QModelIndex &) {
    qDebug() << "current changed";

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
    QAction *start = menu.addAction(Icons::startTimer(), "Start",
                                    [this, project] { _control->startProject(project); });
    start->setIconVisibleInMenu(true);

    QAction *stop = menu.addAction(Icons::stopTimer(), "Stop", [this] { _control->stopActivity(); });
    stop->setIconVisibleInMenu(true);

    menu.addSeparator();
    menu.addAction(Icons::projectNew(), "Create new subproject", [this, project] { createNewProject(project); })->setIconVisibleInMenu(true);
    menu.addAction(Icons::projectRemove(), "Delete project...", [this, project] { ActionUtils::removeProject(_control, project, this); })->setIconVisibleInMenu(true);

    bool started = _control->isStarted(project);
    start->setEnabled(!started);
    stop->setEnabled(started);

    menu.exec(globalPos);
}

void ProjectTreeView::refresh() {
    qDebug() << "refresh";
    reset();
    _proxyModel->invalidate();
    _sourceModel->loadProjects();

    expandToDepth(0);
}

void ProjectTreeView::projectUpdated(const Project &project) {
    qDebug() << "projectUpdated";
    _sourceModel->updateProject(project);
}

void ProjectTreeView::projectsStatusChanged(const QStringList &projectIDs) {
    qDebug() << "projectStatusChanged";
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
