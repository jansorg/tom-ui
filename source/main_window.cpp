#include <QtWidgets/QTreeView>
#include <QtWidgets/QAction>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMainWindow>

#include "version.h"
#include "icons.h"
#include "main_window.h"
#include "model/frametableviewmodel.h"
#include "model/ProjectTreeModel.h"
#include "view/projecttreeview.h"

MainWindow::MainWindow(GotimeControl *control, QMainWindow *parent) : QMainWindow(parent), gotimeControl(control) {
//#ifdef Q_OS_LINUX
    setWindowIcon(QIcon(":/images/logo32.png"));
//#endif

    ui.setupUi(this);
    ui.projectTree->setControl(control);

    ui.actionQuit->setIcon(Icons::exit());
    ui.actionHelpAbout->setIcon(Icons::about());

    connect(ui.projectTree, &ProjectTreeView::projectSelected, this, &MainWindow::loadFrames);
    connect(control, &GotimeControl::projectStarted, this, &MainWindow::projectStatusChanged);
    connect(control, &GotimeControl::projectStopped, this, &MainWindow::projectStatusChanged);

    connect(ui.actionQuit, &QAction::triggered, &QCoreApplication::quit);

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

    if (!project.isValid()) {
        ui.frameView->setModel(nullptr);
        return;
    }

    auto frames = gotimeControl->loadFrames(project.getID(), true);

    auto *sortedModel = new QSortFilterProxyModel(this);
    auto *frameModel = new FrameTableViewModel(frames, gotimeControl, this);
    sortedModel->setSourceModel(frameModel);
    ui.frameView->setModel(sortedModel);
    ui.frameView->sortByColumn(0, Qt::DescendingOrder);
}

void MainWindow::helpAbout() {
    QString about = QString("Tom is a simple UI for the <a href=\"https://github.com/jansorg/tom-ui\">tom time tracker</a> command line application.<br><br>Version: %1").arg(PROJECT_VERSION);
    QMessageBox::about(this, "About Tom", about);
}

void MainWindow::createProject() {

}
