#include <model/frametableviewmodel.h>
#include <QtWidgets/QTreeView>
#include "model/ProjectTreeModel.h"
#include "gotime/startStop_project_action.h"
#include "main_window.h"

MainWindow::MainWindow(GotimeControl *control, QMainWindow *parent) : gotimeControl(control), QMainWindow(parent) {
    ui.setupUi(this);
    ui.projectTree->setControl(control);

    createActions();

    refreshData();
    connect(ui.projectTree, &QTreeView::activated, this, &MainWindow::projectChanged);
    connect(control, &GotimeControl::projectStarted, this, &MainWindow::projectStatusChanged);
    connect(control, &GotimeControl::projectStopped, this, &MainWindow::projectStatusChanged);
}

void MainWindow::refreshData() {
    ui.projectTree->refresh();
}

MainWindow::~MainWindow() = default;

void MainWindow::projectChanged(const QModelIndex &index) {
    auto *item = static_cast<ProjectTreeItem *>(index.internalPointer());
    if (item && item->getProject().isValid()) {
        selectedProject = item->getProject();
        loadFrames(item->getProject());
    }
}

void MainWindow::createActions() {
}

void MainWindow::projectStatusChanged(const Project &project) {
    if (selectedProject.getID() == project.getID()) {
        loadFrames(project);
    }
}

void MainWindow::loadFrames(const Project &project) {
    auto frames = gotimeControl->loadFrames(project.getID(), true);

    auto *sortedModel = new QSortFilterProxyModel(this);
    auto *frameModel = new FrameTableViewModel(frames, gotimeControl, this);
    sortedModel->setSourceModel(frameModel);
    ui.frameView->setModel(sortedModel);
    ui.frameView->sortByColumn(0, Qt::DescendingOrder);
}
