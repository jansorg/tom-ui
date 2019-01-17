#include <model/frametableviewmodel.h>
#include <QtWidgets/QTreeView>
#include "model/ProjectTreeModel.h"
#include "gotime/startStop_project_action.h"
#include "main_window.h"

MainWindow::MainWindow(GotimeControl *control, QMainWindow *parent) : gotimeControl(control), QMainWindow(parent) {
    ui.setupUi(this);
    ui.projectTree->setControl(control);

    connect(ui.projectTree, &ProjectTreeView::projectSelected, this, &MainWindow::loadFrames);
    connect(control, &GotimeControl::projectStarted, this, &MainWindow::projectStatusChanged);
    connect(control, &GotimeControl::projectStopped, this, &MainWindow::projectStatusChanged);

    createActions();
    refreshData();
}

void MainWindow::refreshData() {
    ui.projectTree->refresh();
}

MainWindow::~MainWindow() = default;


void MainWindow::createActions() {
}

void MainWindow::projectStatusChanged(const Project &project) {
    if (_selectedProject.getID() == project.getID()) {
        loadFrames(project);
    }
}

void MainWindow::loadFrames(const Project &project) {
    _selectedProject = project;

    auto frames = gotimeControl->loadFrames(project.getID(), true);

    auto *sortedModel = new QSortFilterProxyModel(this);
    auto *frameModel = new FrameTableViewModel(frames, gotimeControl, this);
    sortedModel->setSourceModel(frameModel);
    ui.frameView->setModel(sortedModel);
    ui.frameView->sortByColumn(0, Qt::DescendingOrder);
}
