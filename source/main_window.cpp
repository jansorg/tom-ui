#include <QtWidgets/QTreeView>
#include <QtWidgets/QAction>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMainWindow>
#include <dialogs/CommonDialogs.h>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QFileDialog>

#include "version.h"
#include "icons.h"
#include "main_window.h"

MainWindow::MainWindow(GotimeControl *control, ProjectStatusManager *statusManager, QMainWindow *parent) : QMainWindow(parent), _control(control) {
//#ifndef Q_OS_MAC
    setWindowIcon(Icons::Logo());
//#endif

    ui.setupUi(this);
    ui.projectTree->setup(control, statusManager);
    ui.frameView->setup(control);

    ui.actionQuit->setIcon(Icons::exit());
    ui.actionHelpAbout->setIcon(Icons::about());

    connect(ui.projectTree, &ProjectTreeView::projectSelected, ui.frameView, &FrameTableView::onProjectSelected);
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

void MainWindow::helpAbout() {
    QString about = QString("Tom is a simple UI for the <a href=\"https://github.com/jansorg/tom-ui\">tom time tracker</a> command line application.<br><br>Version: %1")
            .arg(PROJECT_VERSION);

    QMessageBox::about(this, "About Tom", about);
}

void MainWindow::createProject() {
    CommonDialogs::createProject(Project(), _control, this);
}

void MainWindow::importMacTimeTracker() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Mac Time Tracker CSV export"), "", tr("CSV Files (*.csv)"));

    if (!fileName.isEmpty()) {
        _control->importMacTimeTracker(fileName);
    }
}

void MainWindow::importFanurio() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Fanurio CSV export"), "", tr("CSV Files (*.csv)"));

    if (!fileName.isEmpty()) {
        _control->importFanurioCSV(fileName);
    }
}

void MainWindow::importWatson() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Watson frames file"), "", tr("Watson frames files (frames)"));

    if (!fileName.isEmpty()) {
        _control->importWatsonFrames(fileName);
    }
}

void MainWindow::resetAllData() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Reset data"), tr("Do you want to remove all projects, tags and frames?"), QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        _control->resetAll();
    }
}
