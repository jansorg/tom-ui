#include <QtWidgets/QTreeView>
#include <QtWidgets/QAction>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMainWindow>
#include <dialogs/CommonDialogs.h>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QFileDialog>
#include <source/report/ProjectReportDialog.h>
#include <source/projectlookup/projectlookup.h>

#include "version.h"
#include "icons.h"
#include "main_window.h"
#include "ActionUtils.h"

MainWindow::MainWindow(TomControl *control, ProjectStatusManager *statusManager, QMainWindow *parent) : QMainWindow(parent), _control(control), _statusManager(statusManager) {
//#ifndef Q_OS_MAC
    setWindowIcon(Icons::LogoLarge());
//#endif

    ui.setupUi(this);
    ui.projectTree->setup(control, statusManager);
    ui.frameView->setup(control);

    ui.actionCreateProject->setIcon(Icons::projectNew());

    ui.actionImportMacTimeTracker->setIcon(Icons::importData());
    ui.actionImportFanurio->setIcon(Icons::importData());
    ui.actionImportWatson->setIcon(Icons::importData());

    ui.actionProjectStart->setIcon(Icons::projectStart());
    ui.actionProjectStop->setIcon(Icons::projectStop());
    ui.actionProjectRemove->setIcon(Icons::projectRemove());

    ui.actionTimeEntryRemove->setIcon(Icons::timeEntryDelete());

    connect(ui.projectTree, &ProjectTreeView::projectSelected, ui.frameView, &FrameTableView::onProjectSelected);
    connect(ui.projectTree, &ProjectTreeView::projectSelected, this, &MainWindow::onProjectSelectionChange);

    connect(ui.frameView->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &MainWindow::onEntrySelectionChange);

    connect(_control, &TomControl::projectStarted, this, &MainWindow::onProjectStatusChange);
    connect(_control, &TomControl::projectStopped, this, &MainWindow::onProjectStatusChange);

    connect(ui.actionQuit, &QAction::triggered, &QCoreApplication::quit);
    connect(ui.actionAboutQt, &QAction::triggered, &QApplication::aboutQt);

    createActions();
    refreshData();
}

void MainWindow::refreshData() {
    ui.projectTree->refresh();
    onProjectStatusChange();
    onEntrySelectionChange(QItemSelection());
}

MainWindow::~MainWindow() = default;


void MainWindow::createActions() {
}

void MainWindow::helpAbout() {
    QString about = QString(
            "Tom is a simple UI for the <a href=\"https://github.com/jansorg/tom-ui\">tom time tracker</a> command line application.<br><br>Version: %1")
            .arg(PROJECT_VERSION);

    QMessageBox::about(this, "About Tom", about);
}

void MainWindow::createProject() {
    CommonDialogs::createProject(Project(), _control, this);
}

void MainWindow::importMacTimeTracker() {
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select Mac Time Tracker CSV export"),
                                                    "",
                                                    tr("CSV Files (*.csv)"));

    if (!fileName.isEmpty()) {
        _control->importMacTimeTracker(fileName);
    }
}

void MainWindow::importFanurio() {
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select Fanurio CSV export"),
                                                    "",
                                                    tr("CSV Files (*.csv)"));

    if (!fileName.isEmpty()) {
        _control->importFanurioCSV(fileName);
    }
}

void MainWindow::importWatson() {
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select Watson frames file"),
                                                    "",
                                                    tr("Watson frames files (frames)"));

    if (!fileName.isEmpty()) {
        _control->importWatsonFrames(fileName);
    }
}

void MainWindow::resetAllData() {
    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              tr("Reset data"),
                                                              tr("Do you want to remove all projects, tags and frames?"),
                                                              QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        _control->resetAll();
    }
}

void MainWindow::onProjectSelectionChange(const Project &current) {
    ui.actionProjectStart->setEnabled(current.isValid());
    ui.actionProjectRemove->setEnabled(current.isValid());
}

void MainWindow::onProjectStatusChange() {
    ui.actionProjectStop->setEnabled(_control->status().currentProject().isValid());
}

void MainWindow::startCurrentProject() {
    const Project &project = ui.projectTree->getCurrentProject();
    if (project.isValid()) {
        _control->startProject(project);
    }
}

void MainWindow::stopCurrentProject() {
    _control->stopActivity();
}

void MainWindow::deleteSelectedTimeEntries() {
    ui.frameView->deleteSelectedEntries();
}

void MainWindow::deleteCurrentProject() {
    ActionUtils::removeProject(_control, ui.projectTree->getCurrentProject(), this);
}

void MainWindow::onEntrySelectionChange(const QItemSelection &selection) {
    ui.actionTimeEntryRemove->setEnabled(!selection.isEmpty());
}

void MainWindow::createReport() {
//    QStringList &ids = QStringList() << ui.projectTree->getCurrentProject().getID();
    ProjectReportDialog *dialog = new ProjectReportDialog(QList<Project>(), _control, this);
    dialog->show();
}

void MainWindow::lookupProject() {
    ProjectLookup::show(_control, this);
}
